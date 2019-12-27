#include "AQMonitor.h"
#include "Bme280BoschWrapper.h"

void BoschBME280::begin() {
    sensorFound = bme280.beginI2C(0x77);
    logger.log("BME280 beginI2C result: %s", sensorFound ? "true" : "false");
    this->lastRead = millis() - BME280_READ_INTERVAL + 200;
}

void BoschBME280::loop() {
    if (!sensorFound) {
        return;
    }

    unsigned long timeSinceLastStateUpdate = millis() - this->lastRead;
    if (timeSinceLastStateUpdate > BME280_READ_INTERVAL) {
        lastRead = millis();
        bool ok = bme280.measure();
        if (ok) {
            temp = bme280.getTemperature() / 100.0;  // 100 * C
            humidity = bme280.getHumidity() / 1024.0;  // 1024 * % relative humidity
            pressure = bme280.getPressure() / 100000;  // From 100 * Pa to hPa/mbar
        } else {
            logger.log("Failed on BME280 .measure()");
        }
    }
}

float BoschBME280::getTemperature() {
    return this->temp + settings.getSettings()->bme280.temperatureOffset / 10.0;
}

float BoschBME280::getHumidity() {
    float correctedTemp = getTemperature();
    float ah = getAbsoluteHimidity();

    // Binary search to find the correct humidity. Seems to be the simplest solution. 10
    // steps guarantee 0.1 precision.
    float l = 0.0f;
    float r = 100.0f;

    for (int i = 0; i < 10; i++) {
        float m = l + (r-l) / 2;
        float ah_at_m = rhToAh(m, correctedTemp);
        if ( ah_at_m < ah) {
            l = m;
        } else if (ah_at_m > ah) {
            r = m;
        } else {
            break;
        }
    }

    return l + (r-l) / 2;
}

int BoschBME280::getPressure() {
    return this->pressure + settings.getSettings()->bme280.pressureOffset;
}

float BoschBME280::getAbsoluteHimidity() {
    return rhToAh(this->humidity, this->temp);
}

void BoschBME280::get_config_page(char* buffer) {
    sprintf_P(
        buffer,
        BME280_CONFIG_PAGE,
        settings.getSettings()->bme280.temperatureOffset,
        settings.getSettings()->bme280.pressureOffset);
}

void BoschBME280::parse_config_params(WebServerBase* webServer, bool& save) {
    webServer->process_setting("temp_offset", settings.getSettings()->bme280.temperatureOffset, save);
    webServer->process_setting("pressure_offset", settings.getSettings()->bme280.pressureOffset, save);
}

float BoschBME280::rhToAh(float rh, float temp) {
    // https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
    double p_sat = 6.112 * pow(EULER, (17.67 * temp) / (temp + 243.5));
    return (p_sat * rh * 2.167428434) / (273.15 + temp);
}
