//#include <unistd.h>
#include <QtNetwork/QNetworkAccessManager>
#include <QByteArray>
#include <qvector.h>

#include <QJsonValue>
#include <QJsonDocument>
#include <QJsonObject>
#include <QVariantMap>
#include <QJsonArray>

#include "TP5_WeatherStation.h"
#include "ui_TP5_WeatherStation.h"
#include <iostream>

#include "weatherreport.h"
using namespace std;



TP5_WeatherStation::TP5_WeatherStation(DbManager *dbm, QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::TP5_WeatherStationClass)
    , weatherReport (new WeatherReport) // Weather Data class
    , dbmanager (dbm)                   // DB Manager, for Pollution Data
{
    ui->setupUi(this);

    // Weather report View
    reportView = new ViewReport(weatherReport,ui);
    // Pollution Forecast View
    pollutionView = new ViewPollution(dbmanager, ui->groupBox_pollution);

    // comment following line and uncomment next one, once observable implemented
    netmanager = new QNetworkAccessManager(this);
    netmanager2 = new QNetworkAccessManager(this);
    connect(netmanager, SIGNAL(finished(QNetworkReply*)), this, SLOT(weatherReplyFinished(QNetworkReply*)));
    connect(netmanager2, SIGNAL(finished(QNetworkReply*)), this, SLOT(polutionReplyFinished(QNetworkReply*)));
    weatherRequest();
    pollutionRequest();
    connect(ui->pushButton_weather_request, &QPushButton::pressed, this, &TP5_WeatherStation::weatherRequest);
    connect(ui->pushButton_weather_request, &QPushButton::pressed, this, &TP5_WeatherStation::pollutionRequest);

}

TP5_WeatherStation::~TP5_WeatherStation()
{
    netmanager->deleteLater();
    netmanager2->deleteLater();
    delete ui;
    delete dbmanager;
}

void TP5_WeatherStation::weatherRequest() {

    // your netmanager and request here
    QString URL = "https://api.openweathermap.org/data/2.5/weather?q=bourg-en-bresse&lang=fr&units=metric&appid=d0562f476913da692a065c608d0539f6";
    QUrl url(URL);
    QNetworkRequest request;
    request.setUrl(url);
    //--header ’Accept: application/json’
    request.setRawHeader("Accept", "application/json");
    qDebug() << Q_FUNC_INFO << request.url();
    netmanager->get(request);
}

void TP5_WeatherStation::pollutionRequest() {
    // your netmanager and request here
    QString URL = "https://api.openweathermap.org/data/2.5/air_pollution/forecast?lat=46.1667&lon=5.1667&units=metric&lang=fr&appid=d0562f476913da692a065c608d0539f6";
    QUrl url(URL);
    QNetworkRequest request;
    request.setUrl(url);
    //--header ’Accept: application/json’
    request.setRawHeader("Accept", "application/json");
    qDebug() << Q_FUNC_INFO << request.url();
    netmanager2->get(request);


}

void TP5_WeatherStation::weatherReplyFinished(QNetworkReply* reply)
{
    // managing response here (with errors handling if possible)
    if (reply->error() != QNetworkReply::NoError)
    {
        //Network Error
        qDebug() << reply->error() << "=>" << reply->errorString();
    }
    else if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    { 
        cout << "connection API faite" << endl;
        QByteArray datas = reply->readAll();
        QString infos(datas);
        QJsonDocument jsonResponse = QJsonDocument::fromJson(infos.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();


        QJsonObject mainObj = jsonObj["main"].toObject();
        double temp = mainObj["temp"].toDouble();
        double temp_min = mainObj["temp_min"].toDouble();
        double temp_max = mainObj["temp_max"].toDouble();
        

        QJsonObject coordObj = jsonObj["coord"].toObject();
        double lat = coordObj["lat"].toDouble();
        double lon = coordObj["lon"].toDouble();



        QJsonArray weatherArray = jsonObj["weather"].toArray();
        QJsonObject data = weatherArray[0].toObject();
        QString main = data["main"].toString();
        QString desc = data["description"].toString();


        weatherReport->SetWeatherReport(main, desc, temp, temp_min, temp_max, lon, lat);

        reportView->update();

    } 
    else { 
        cout << "fail to connect API" << endl;
    }

    //don't forget to delete when no more requested
    reply->deleteLater();
}


void TP5_WeatherStation::polutionReplyFinished(QNetworkReply* reply)
{
    // managing response here (with errors handling if possible)
    if (reply->error() != QNetworkReply::NoError)
    {
        //Network Error
        qDebug() << reply->error() << "=>" << reply->errorString();
    }
    else if (reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() == 200)
    {
        cout << "connection API polution faite" << endl;


        QByteArray datas = reply->readAll();
        QString infos(datas);
        QJsonDocument jsonResponse = QJsonDocument::fromJson(infos.toUtf8());
        QJsonObject jsonObj = jsonResponse.object();

        QJsonArray listArray = jsonObj["list"].toArray();
        QJsonObject obj;

        dbmanager->removeAllData();

        for (int i = 0; i < listArray.size(); i++) {
            obj = listArray[i].toObject();

            qint64 dt = obj["dt"].toInt();
            QDateTime localTime = QDateTime::fromSecsSinceEpoch(dt); // s to local
            qint64 msdt = localTime.toMSecsSinceEpoch();

            QJsonObject main = obj["main"].toObject();
            int aq = main["aqi"].toInt();
        
            dbmanager->addData(msdt,aq);
        }
        pollutionView->update();



    }
    else {
        cout << "fail to connect API" << endl;
    }

    //don't forget to delete when no more requested
    reply->deleteLater();
}