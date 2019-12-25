#pragma once

#include "SettingsBase.h"
#include "WiFi.h"
#include "InfluxDBCollector.h"

struct SettingsData {
    NetworkSettings network;
    InfluxDBCollectorSettings influxDB;
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
