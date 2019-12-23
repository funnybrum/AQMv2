#include "DataCollector.h"
#include "AQMonitor.h"

int lastCO2 = -1;
int lastPushedCO2 = -1;
float lastTemp = -1;
float lastPushedTemp = -1;


void collectData(InfluxDBCollector* collector) {
    lastCO2 = co2.getCO2();
    lastTemp = tempSensor.getTemperature() * 10;
    if (lastPushedCO2 < 0) {
        lastPushedCO2 = lastCO2;
        lastPushedTemp = lastTemp;
    }
    collector->append("co2", lastCO2);
    collector->append("temperature", tempSensor.getTemperature(), 2);
    collector->append("humidity", tempSensor.getHumidity(), 1);
    collector->append("pressure", tempSensor.getPressure());
}

void onPush() {
    lastPushedCO2 = lastCO2;
    lastPushedTemp = lastTemp;
}

bool shouldPush() {
    if (lastPushedCO2 < 0) {
        return false;
    }

    if ((lastPushedCO2/lastCO2 <= 0.8f || lastPushedCO2/lastCO2 >= 1.25f)) {
        return true;
    }

    if (abs(lastPushedTemp - lastTemp) > 1.0) {
        return true;
    }

    return false;
}

// TODO: Try to extend the InfluxDBCollector.
void DataCollector::begin() {
    // TODO: move the callbacks as class members and use something like std::bind (as in
    // WebServer.cpp). i.e. std::bind(&DataCollector::onPush, this)
    influxDBCollector = new InfluxDBCollector(&logger,
                                              &wifi,
                                              &settingsData.influxDB,
                                              &settingsData.network,
                                              collectData,
                                              shouldPush,
                                              onPush);
    influxDBCollector->begin();
}

void DataCollector::loop() {
    influxDBCollector->loop();
}

void DataCollector::get_config_page(char* buf) {
    influxDBCollector->get_config_page(buf);
}

void DataCollector::parse_config_params(WebServerBase* webServer, bool& save) {
    influxDBCollector->parse_config_params(webServer, save);
}