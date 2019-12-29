#include "AQMonitor.h"
#include "DataCollector.h"

DataCollector::DataCollector():
    InfluxDBCollector(&logger,
                      &wifi,
                      &settings.getSettings()->influxDB,
                      &settings.getSettings()->network) {
}

bool DataCollector::shouldCollect() {
    return co2.getCO2() > 0 && tempSensor.getPressure() > 100;
}

void DataCollector::collectData() {
    lastCO2 = co2.getCO2();
    lastTemp = tempSensor.getTemperature() * 10;
    if (lastPushedCO2 < 0) {
        lastPushedCO2 = lastCO2;
        lastPushedTemp = lastTemp;
    }
    append("co2", co2.getCO2());
    append("temperature", tempSensor.getTemperature(), 2);
    append("humidity", tempSensor.getHumidity(), 1);
    append("abs_humidity", tempSensor.getAbsoluteHimidity(), 2);
    append("pressure", tempSensor.getPressure(), 1);
    append("uptime", millis() / 1000);
}

bool DataCollector::shouldPush() {
    if (lastPushedCO2 < 0) {
        return false;
    }

    if (lastCO2 > 0 && (lastPushedCO2/lastCO2 <= 0.8f || lastPushedCO2/lastCO2 >= 1.25f)) {
        return true;
    }

    if (abs(lastPushedTemp - lastTemp) > 1.0) {
        return true;
    }

    return false;
}

void DataCollector::onPush() {
    lastPushedCO2 = lastCO2;
    lastPushedTemp = lastTemp;
}