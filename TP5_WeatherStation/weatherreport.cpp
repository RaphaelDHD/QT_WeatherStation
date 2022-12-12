#include "weatherreport.h"
#include <QDateTime>
#include <QTimeZone>
//#include <unistd.h>

WeatherReport::WeatherReport()
    : temp(0.0),temp_min(0.0),temp_max(0.0),
      lon(0.0),lat(0.0)
{ }

void WeatherReport::SetWeatherReport(QString mn, QString des ,double tmp, double tmp_min, double tmp_max, double lo, double la) {
    main = mn;
    description = des;
    temp = tmp;
    temp_min = tmp_min;
    temp_max = tmp_max;
    lon = lo;
    lat = la;
}