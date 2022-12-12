#include "TP5_WeatherStation.h"
#include <QtWidgets/QApplication>
#include <QDebug>
#include <QDir>
#include "dbmanager.h"

int main(int argc, char *argv[])
{

    // The DB Model
    DbManager* dbm = new DbManager(QDir::currentPath() + "/../pollution.db");

    // Example on how to use the DB Manager
    if (dbm->isOpen())
    {   // Creates a table if it doesn't exist.
        // Otherwise, use existing table.

        qDebug() << "End";
    }
    else
    {
        qDebug() << "Database is not open!";
    }

    // The View
    QApplication a(argc, argv);
    TP5_WeatherStation w(dbm); // dbm deleted in ~TP5_WeatherStation
    w.show();
    return a.exec();
}
