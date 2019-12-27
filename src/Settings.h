#pragma once

#include "SettingsBase.h"
#include "WiFi.h"
#include "InfluxDBCollector.h"
#include "BME280.h"

struct SettingsData {
    NetworkSettings network;
    InfluxDBCollectorSettings influxDB;
    BME280Settings bme280;
    struct AQSensor {
        int16_t temperatureOffset;
        int16_t humidityOffset;
    } aqSensor;
};

class Settings: public SettingsBase<SettingsData> {
    public:
        Settings();
        SettingsData* getSettings();

    protected:
        void initializeSettings();

    private:
        SettingsData settingsData;
};
