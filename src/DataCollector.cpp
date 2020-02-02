#include "AQMonitor.h"
#include "DataCollector.h"

DataCollector::DataCollector():
    InfluxDBCollector(&logger,
                      &wifi,
                      &settings.getSettings()->influxDB,
                      &settings.getSettings()->network) {
}

bool DataCollector::shouldCollect() {
    return tempSensor.getPressure() > 100 ||
           pm.getPM10() > 0 ||
           co2.getCO2() > 0;
}

void DataCollector::collectData() {
    if (co2.getCO2() > 0) {
        append("co2", co2.getCO2());

        lastCO2 = co2.getCO2();
        if (lastPushedCO2 < 0) {
            lastPushedCO2 = co2.getCO2(); 
        }
    }
    if (pm.getPM10() > 0) {
        append("pm_1.0", pm.getPM01());
        append("pm_2.5", pm.getPM02());
        append("pm_10", pm.getPM10());
    }
    if (tempSensor.getPressure() > 100) {
        append("temperature", tempSensor.getTemperature(), 2);
        append("humidity", tempSensor.getHumidity(), 1);
        append("abs_humidity", tempSensor.getAbsoluteHimidity(), 2);
        append("pressure", tempSensor.getPressure(), 1);

        if (lastPushedHumidity < 0) {
            lastPushedTemp = tempSensor.getTemperature();
            lastPushedHumidity = tempSensor.getHumidity();
        }
        lastTemp = tempSensor.getTemperature();
        lastHumidity = tempSensor.getHumidity();
    }
}

bool DataCollector::shouldPush() {
    float co2ratio = 1.0 * lastPushedCO2 / lastCO2;
    if (lastPushedCO2 > 0 && (co2ratio <= 0.8f || co2ratio >= 1.2f)) {
        return true;
    }

    if (lastPushedHumidity > 0) {
        if (fabsf(lastPushedTemp - lastTemp) > 1.0) {
            return true;
        }

        if (fabsf(lastPushedHumidity - lastHumidity) > 5.0) {
            return true;
        }
    }

    return false;
}   

void DataCollector::onPush() {
    lastPushedCO2 = lastCO2;
    lastPushedTemp = lastTemp;
    lastPushedHumidity = lastHumidity;
}