#include "viewpollution.h"
#include <QDebug>
#include <QDateTime>

ViewPollution::ViewPollution(DbManager* dbm, QWidget* widget) :dbm(dbm), widget(widget)
{
    series = new QLineSeries();

    QChart *chart = new QChart();
    chart->legend()->hide();
    chart->addSeries(series);
    chart->setTitle("Simple date/line chart example");
    chart->setAnimationOptions(QChart::SeriesAnimations);

    //    chart->createDefaultAxes();
    axisX = new QDateTimeAxis;
    axisX->setTickCount(10);
    axisX->setFormat("HH:mm");
    axisX->setTitleText("Date");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    axisY = new QValueAxis;
    axisY->setLabelFormat("%i");
    axisY->setTitleText("Air Quality Indice");
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    chart->setTheme(QChart::ChartTheme(2));
    chart->setAnimationOptions(QChart::AnimationOption(1));

    QChartView *chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    QGridLayout *chartLayout = new QGridLayout;
    chartLayout->addWidget(chartView, 0, 0);
    chartLayout->setContentsMargins(0, 0, 0, 0);




    // update chart
    widget->setLayout(chartLayout);
    update();
}


void ViewPollution::update()
{


    QVector<qint64> time = dbm->getDt();
    QVector<int> aqi = dbm->getAqi();
    series->clear();

    for (int i = 0; i < time.size(); i++) {
        series->append(time[i], aqi[i]);
    }

    QDateTime dt0;
    dt0.setMSecsSinceEpoch(time[0]); //xmin for mock values
    QDateTime dt1;
    dt1.setMSecsSinceEpoch(time[time.size() - 1]); //xmax
    axisX->setRange(dt0, dt1);
    axisY->setRange(-5, 5); //ymin,ymax (do not change for these mock values)

    widget->repaint();
}
