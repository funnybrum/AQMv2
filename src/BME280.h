#pragma once

#include "Bme280BoschWrapper.h"

#define BME280_READ_INTERVAL 5000

struct BME280Settings {
    int8_t temperatureOffset;  // In 0.1C
    int8_t pressureOffset;     // In mbar
};

const char BME280_CONFIG_PAGE[] PROGMEM = R"=====(
<fieldset style='display: inline-block; width: 300px'>
<legend>BME280 settings</legend>
Temperature offset:<br>
<input type="text" name="temp_offset" value="%d"><br>
<small><em>in 0.1 degrees, from -125 to 125</em></small><br><br>
Pressure offset:<br>
<input type="text" name="pressure_offset" value="%d"><br>
<small><em>in mbar, from -125 to 125</em></small><br><br>
</fieldset>
)=====";

class BoschBME280 {
    public:
        void begin();
        void loop();

        float getTemperature();
        float getHumidity();
        int getPressure();
        float getAbsoluteHimidity();

        void get_config_page(char* buffer);
        void parse_config_params(WebServerBase* webServer, bool& save);

    private:
        float rhToAh(float rh, float temp);

        float temp;
        float humidity;
        int pressure;
        unsigned long lastRead;
        bool sensorFound;
        Bme280BoschWrapper bme280 = Bme280BoschWrapper(true);
};
