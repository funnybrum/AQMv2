#pragma once

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <ESP8266mDNS.h>

#include "user_interface.h"

#include "esp8266-base.h"

extern Logger logger;
extern Settings settings;
extern WiFiManager wifi;

#include "BoschBME280.h"
#include "S8.h"
#include "PMSx003.h"
#include "DataCollector.h"

extern BoschBME280 tempSensor;
extern SensairS8 co2;
extern PlantowerPMSx003 pm;
extern DataCollector dataCollector;

#define HTTP_PORT 80
#define HOSTNAME "aq-monitor"