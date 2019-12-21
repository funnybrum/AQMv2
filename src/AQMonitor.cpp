#include "AQMonitor.h"

void initSettings() {
    strcpy(settingsData.network.hostname, HOSTNAME);
}

SettingsData settingsData = SettingsData();
Logger logger = Logger(false);
Settings settings = Settings(&logger, (void*)(&settingsData), sizeof(SettingsData), initSettings);
WiFiManager wifi = WiFiManager(&logger, &settingsData.network);
WebServer webServer = WebServer(&settingsData.network, &logger);
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

    // pinMode(12, OUTPUT);
    // pinMode(13, OUTPUT);
    // pinMode(14, OUTPUT);
}

// int counter = 0;

void loop() {
    wifi.loop();
    webServer.loop();
    settings.loop();
    tempSensor.loop();
    co2.loop();

    // if (counter == 0) {
    //     digitalWrite(12, HIGH);
    //     digitalWrite(13, HIGH);
    //     digitalWrite(14, HIGH);
    //     // logger.log("All on!");
    // }

    // if (counter == 20) {
    //     digitalWrite(12, LOW);
    //     digitalWrite(13, LOW);
    //     digitalWrite(14, LOW);
    //     // logger.log("All off!");
    // }

    // if (counter >= 20) {
    //     counter = -1;
    // }

    // counter++;

    // logger.log("Count: %d", counter);

    delay(100);

}
