#pragma once

#include "Bme280BoschWrapper.h"

#define BME280_READ_INTERVAL 5000

struct BME280Settings {
    int8_t temperatureOffset;  // In 0.1C
    int8_t pressureOffset;     // In mbar
    int16_t humidityFactor;    // In 0.01 units, i.e. 100 here sets the factor to 1
    int8_t humidityOffset;     // In 0.1%
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
<br>
Humidity correction:<br>
<small>Used if sensor can't be reconditioned<br>corrected = 0.01*factor*raw + 0.1*offset</small><br>
Humidity factor:<br>
<input type="text" name="humidity_factor" value="%d"><br>
Humidity offset:<br>
<input type="text" name="humidity_offset" value="%d"><br>
</fieldset>
)=====";

class BoschBME280 {
    public:
        void begin();
        void loop();

        float getTemperature();
        float getRawTemperature();
        float getHumidity();
        float getRawHumidity();
        float getPressure();
        float getAbsoluteHimidity();

        void get_config_page(char* buffer);
        void parse_config_params(WebServerBase* webServer, bool& save);

    private:
        float rhToAh(float rh, float temp);

        float temp;
        float humidity;
        float raw_temp;
        float raw_humidity;
        int pressure;
        unsigned long lastRead;
        bool sensorFound;
        Bme280BoschWrapper bme280 = Bme280BoschWrapper(true);
};
