#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include "user_interface.h"

#include "esp8266-base.h"

#include "BME280.h"
#include "S8.h"
#include "DataCollector.h"

#define HTTP_PORT 80
#define HOSTNAME "aq-monitor"

struct SettingsData {
    NetworkSettings network;
    struct InfluxDBCollectorSettings influxDB;
    struct AQSensor {
        int16_t temperatureOffset;
        int16_t humidityOffset;
    } aqSensor;
};

extern Logger logger;
extern Settings settings;
extern SettingsData settingsData;
extern WiFiManager wifi;
extern BoschBME280 tempSensor;
extern SensairS8 co2;
extern DataCollector dataCollector;
