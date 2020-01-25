#include "AQMonitor.h"

Logger logger = Logger(false);
Settings settings = Settings();

WiFiManager wifi = WiFiManager(&logger, &settings.getSettings()->network);
WebServer webServer = WebServer(&logger, &settings.getSettings()->network);

DataCollector dataCollector = DataCollector();
BoschBME280 tempSensor = BoschBME280();
SensairS8 co2 = SensairS8();
PlantowerPMSx003 pm = PlantowerPMSx003();

void setup()
{ 
    settings.begin();

    wifi.begin();
    wifi.connect();

    webServer.begin();

    tempSensor.begin();
    co2.begin();
    pm.begin();
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
    pm.loop();
    dataCollector.loop();

    delay(100);
}
