#pragma once

#include "AQMonitor.h"
#include "cactus_io_BME280_I2C.h"

#define BME280_READ_INTERVAL 5000

// #define BME_DEBUG

class BoschBME280 {
    public:
        float getTemperature() {
            return this->temp;
        }

        float getHumidity() {
            return this->humidity;
        }

        int getPressure() {
            return this->pressure;
        }

        void begin() {
            sensorFound = this->bme280.begin(&logger);
            logger.log("BME280 begin result: %s", sensorFound ? "true" : "false");
            this->lastRead = millis() - BME280_READ_INTERVAL + 200;
        }

        void loop() {
            if (!sensorFound) {
                return;
            }

            unsigned long timeSinceLastStateUpdate = millis() - this->lastRead;
            if (timeSinceLastStateUpdate > BME280_READ_INTERVAL) {
                lastRead = millis();
                #ifdef BME_DEBUG
                    logger.log("[BME280] Reading temp...");
                #endif
                delay(100);
                this->bme280.readSensor();
                temp = this->bme280.getTemperature_C();
                humidity = this->bme280.getHumidity();
                pressure = this->bme280.getPressure_MB();

                #ifdef BME_DEBUG
                    logger.log("Temp: %.1f", this->temp);
                    logger.log("Humidity: %.0f", this->humidity);
                    logger.log("Pressure: %d", this->pressure);
                #endif
            }
        }

    private:
        float temp;
        float humidity;
        int pressure;
        unsigned long lastRead;
        bool sensorFound;
        BME280_I2C bme280 = BME280_I2C();
};
