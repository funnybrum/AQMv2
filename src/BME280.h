#pragma once

#include "AQMonitor.h"
#include "Bme280BoschWrapper.h"

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
            sensorFound = bme280.beginI2C(0x77);
            logger.log("BME280 beginI2C result: %s", sensorFound ? "true" : "false");
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
                bool ok = bme280.measure();
                if (ok) {
                    temp = bme280.getTemperature() / 100.0;  // 100 * C
                    humidity = bme280.getHumidity() / 1024.0;  // 1024 * % relative humidity
                    pressure = bme280.getPressure() / 100;  // From Pa to millibar
                } else {
                    logger.log("Failed on BME280 .measure()");
                }

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
        Bme280BoschWrapper bme280 = Bme280BoschWrapper(true);
};
