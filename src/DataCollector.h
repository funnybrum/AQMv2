#pragma once

#include "InfluxDBCollector.h"

class DataCollector: public InfluxDBCollector {
    public:
        DataCollector();
        bool shouldCollect();
        void collectData();
        void onPush();
        bool shouldPush();
    private:
        int lastCO2 = -1;
        int lastPushedCO2 = -1;
        float lastTemp = -1;
        float lastPushedTemp = -1;
        float lastHumidity = -1;
        float lastPushedHumidity = -1;
};
