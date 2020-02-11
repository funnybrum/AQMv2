#include "AQMonitor.h"
#include "Wire.h"

bool BoschBME280::begin(uint8_t addr) {
    Wire.begin();
    // I2C configuration.
    bme280.intf = BME280_I2C_INTF;
    bme280.write = BoschBME280::I2CWrite;
    bme280.read = BoschBME280::I2CRead;
    bme280.delay_ms = BoschBME280::delay;
    bme280.dev_id = addr;

    // Oversampling configuration.
    bme280.settings.osr_h = BME280_OVERSAMPLING_1X;
    bme280.settings.osr_p = BME280_OVERSAMPLING_1X;
    bme280.settings.osr_t = BME280_OVERSAMPLING_2X;
    bme280.settings.filter = BME280_FILTER_COEFF_OFF;

    // Initialize the sensor, apply the configuration and set the sensor to FORCED mode.
    errors += bme280_init(&bme280) != BME280_OK;
    errors += (bme280_set_sensor_settings(BME280_OSR_TEMP_SEL|BME280_OSR_HUM_SEL|BME280_FILTER_SEL, &bme280) != BME280_OK) << 1;
    errors += (bme280_set_sensor_mode(BME280_FORCED_MODE, &bme280) != BME280_OK) << 2;
    req_delay = bme280_cal_meas_delay(&bme280.settings);

    if (errors > 0) {
        logger.log("BME280 not found on 0x%02X. Error code is %d", addr, errors);
    }

    // Do the next read in 200ms.
    this->lastRead = millis() - BME280_READ_INTERVAL + 200;

    // Correct the humidity factor on empty settings.
    if (settings.getSettings()->bme280.humidityFactor == 0) {
        // Set non-zero default values.
        settings.getSettings()->bme280.humidityFactor = 100;
        settings.getSettings()->bme280.humidityOffset = 0;
    }

    return errors == 0;
}

void BoschBME280::loop() {
    if (errors > 0) {
        return;
    }

    unsigned long timeSinceLastStateUpdate = millis() - this->lastRead;
    if (timeSinceLastStateUpdate > BME280_READ_INTERVAL) {
        lastRead = millis();
        errors += (bme280_set_sensor_mode(BME280_FORCED_MODE, &bme280) != BME280_OK) << 3;
        bme280.delay_ms(req_delay);
        errors += (bme280_get_sensor_data(BME280_ALL, &comp_data, &bme280) != BME280_OK) << 4;
        if (errors == 0) {
            raw_temp = temp = comp_data.temperature / 100.0;  // Result is 100 * C
            raw_humidity = humidity = comp_data.humidity / 1024.0;  // Result is 1024 * % relative humidity
            pressure = comp_data.pressure / 10000.0;  // Result is in 100 * Pa. Convert it to mbar

            // Apply corrections
            // 1. Apply relative humidity factor and offset. This is for BME280 sensors that still
            // don't provide correct relative humidity after reconditioning.
            humidity = settings.getSettings()->bme280.humidityFactor * humidity * 0.01f +
                       settings.getSettings()->bme280.humidityOffset * 0.1f;

            if (humidity < 0 || humidity > 100) {
                logger.log("Incorrect humidity correction, got %f.", humidity);
                humidity = max(humidity, 0.0f);
                humidity = min(humidity, 100.0f);
            }

            // 2. Get absolute humidity. This will be used to correct relative humidity below
            // after temperature is corrected.
            float ah = getAbsoluteHimidity();

            // 3. Temperature offset. This is not for BME280 issues, but for sensor positioning
            // correction. I.e. if the sensor is near the ceiling - the temperature would be a bit
            // higher.
            temp = temp + settings.getSettings()->bme280.temperatureOffset / 10.0;

            // TODO: Temporary disabled till the correct mechanism for applying corrections has
            // been identified.
            // 4. Correct humidity based on temperature offset. 
            // Binary search to find the correct humidity. Seems to be the simplest solution. 10
            // steps guarantee 0.1 precision. End goal - keep the same absolute humidity after
            // applying temperature correction.
            // float l = 0.0f;
            // float r = 100.0f;

            // for (int i = 0; i < 10; i++) {
            //     float m = l + (r-l) / 2;
            //     float ah_at_m = rhToAh(m, temp);
            //     if ( ah_at_m < ah) {
            //         l = m;
            //     } else if (ah_at_m > ah) {
            //         r = m;
            //     } else {
            //         break;
            //     }
            // }

            // humidity =  l + (r-l) / 2;

            // 5. Pressure offset.
            pressure = pressure + settings.getSettings()->bme280.pressureOffset;
        } else {
            logger.log("BME280 failed on measure. Error code is %d", errors);
        }
    }
}

float BoschBME280::getTemperature() {
    return temp;
}

float BoschBME280::getHumidity() {
    return humidity;
}

float BoschBME280::getRawTemperature() {
    return raw_temp;
}

float BoschBME280::getRawHumidity() {
    return raw_humidity;
}

float BoschBME280::getPressure() {
    return pressure;
}

float BoschBME280::getAbsoluteHimidity() {
    return rhToAh(this->humidity, this->temp);
}

void BoschBME280::get_config_page(char* buffer) {
    sprintf_P(
        buffer,
        BME280_CONFIG_PAGE,
        settings.getSettings()->bme280.temperatureOffset,
        settings.getSettings()->bme280.pressureOffset,
        settings.getSettings()->bme280.humidityFactor,
        settings.getSettings()->bme280.humidityOffset);
}

void BoschBME280::parse_config_params(WebServerBase* webServer, bool& save) {
    webServer->process_setting("temp_offset", settings.getSettings()->bme280.temperatureOffset, save);
    webServer->process_setting("pressure_offset", settings.getSettings()->bme280.pressureOffset, save);
    webServer->process_setting("humidity_factor", settings.getSettings()->bme280.humidityFactor, save);
    webServer->process_setting("humidity_offset", settings.getSettings()->bme280.humidityOffset, save);
}

float BoschBME280::rhToAh(float rh, float temp) {
    // https://carnotcycle.wordpress.com/2012/08/04/how-to-convert-relative-humidity-to-absolute-humidity/
    double p_sat = 6.112 * pow(EULER, (17.67 * temp) / (temp + 243.5));
    return (p_sat * rh * 2.167428434) / (273.15 + temp);
}

int8_t BoschBME280::I2CWrite(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t cnt)
{  
    Wire.beginTransmission(dev_addr);
    Wire.write(reg_addr);
    Wire.write(reg_data, cnt);
    Wire.endTransmission();
    
    return BME280_OK;
}

int8_t BoschBME280::I2CRead(uint8_t dev_addr, uint8_t reg_addr, uint8_t *reg_data, uint16_t cnt)
{
    int8_t ret = BME280_OK;

    Wire.beginTransmission(dev_addr);
    
    Wire.write(reg_addr);
    Wire.endTransmission();
    
    Wire.requestFrom((int)dev_addr, (int)cnt);
    
    uint8_t available = Wire.available();
    if(available != cnt) {
        ret = BME280_E_COMM_FAIL;
    }
    
    for(uint8_t i = 0; i < available; i++) {
        if(i < cnt) {
            *(reg_data + i) = Wire.read();
        }
        else {
            Wire.read();
        }
    }

    return ret;
}

void BoschBME280::delay(unsigned int ms) {
    delay(ms);
}