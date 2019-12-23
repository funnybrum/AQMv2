#pragma once

#include "InfluxDBCollector.h"

class DataCollector {
    public:
        void begin();
        void loop();
        void get_config_page(char* buf);
        void parse_config_params(WebServerBase* webServer, bool& save);

    private:
        InfluxDBCollector *influxDBCollector;
};