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
    lastTemp = tempSensor.getTemperature();
    lastHumidity = tempSensor.getHumidity();
    if (lastPushedCO2 < 0) {
        lastPushedCO2 = lastCO2;
        lastPushedTemp = lastTemp;
        lastPushedHumidity = lastHumidity;
    }
    append("co2", co2.getCO2());
    append("temperature", tempSensor.getTemperature(), 2);
    append("humidity", tempSensor.getHumidity(), 1);
    append("abs_humidity", tempSensor.getAbsoluteHimidity(), 2);
    append("pressure", tempSensor.getPressure(), 1);
    // For debug purposes. There seems to be an issue leading to restarts.
    append("uptime", millis() / 1000);
    append("free_heap", ESP.getFreeHeap());
    append("ifx_ptr", telemetryDataSize);
}

bool DataCollector::shouldPush() {
    if (lastPushedCO2 < 0) {
        return false;
    }

    float co2ratio = 1.0 * lastPushedCO2 / lastCO2;
    if (co2ratio <= 0.8f || co2ratio >= 1.2f) {
        return true;
    }

    if (fabsf(lastPushedTemp - lastTemp) > 1.0) {
        return true;
    }

    if (fabsf(lastPushedHumidity - lastHumidity) > 5.0) {
        return true;
    }

    return false;
}   

void DataCollector::onPush() {
    lastPushedCO2 = lastCO2;
    lastPushedTemp = lastTemp;
    lastPushedHumidity = lastHumidity;
}