#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

SettingsData settingsData = SettingsData();
Logger logger = Logger(false);
Settings settings = Settings(&logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);
WiFiManager wifi = WiFiManager(&logger, &settingsData.network);
WebServer webServer = WebServer(&settingsData.network, &logger);

DataCollector dataCollector = DataCollector();
BoschBME280 tempSensor = BoschBME280();
SensairS8 co2 = SensairS8();

void setup()
{ 
    settings.begin();

    wifi.begin();
    wifi.connect();

    webServer.begin();

    tempSensor.begin();
    co2.begin();
    dataCollector.begin();

    // pinMode(12, OUTPUT);
    // pinMode(13, OUTPUT);
    // pinMode(14, OUTPUT);
}

void loop() {
    wifi.loop();
    webServer.loop();
    settings.loop();

    tempSensor.loop();
    co2.loop();
    dataCollector.loop();

    if (settingsData.influxDB.enable) {
        dataCollector.start();
    } else {
        dataCollector.stop();
    }


    delay(100);
}
