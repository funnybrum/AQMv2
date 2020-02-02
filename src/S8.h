#pragma once

#include "AQMonitor.h"

#define CO2_READ_INTERVAL 5000
// #define CO2_DEBUG
#ifndef CO2_DEBUG
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif

/**
 * Very basic Sensair S8 driver. Pull the CO2 values from the sensor based on predefined interval.
 * The sensor must be connected on the default serial pins of the microcontroller.
 */

class SensairS8 {
    public:
        void begin() {
            this->lastRead = 0;
            Serial.begin(9600);
            #ifdef CO2_DEBUG
            	logger.log("[Sensair S8] Serial started at 9600bps");
            #endif
            inputPos = 0;
        }

        void loop() {
            // If CO2_READ_INTERVAL has passed - send a CO2 read command.
            unsigned long timeSinceLastStateUpdate = millis() - this->lastRead;
            if (timeSinceLastStateUpdate > CO2_READ_INTERVAL) {
                lastRead = millis();

                uint8_t read_CO2_cmd[] = {0xFE, 0x04, 0x00, 0x03, 0x00, 0x01, 0xD5, 0xC5};
                int bytesSent = Serial.write(read_CO2_cmd, sizeof(read_CO2_cmd));
                Serial.flush();
                #ifdef CO2_DEBUG
                    logger.log("[Sensair S8] Read CO2 command sent, %d bytes", bytesSent);
                #endif
            }

            // Process any available bytes on the Serial input.
            while (Serial.available() > 0) {
                int in_byte = Serial.read();
                #ifdef CO2_DEBUG
                    logger.log("[Sensair S8] Got char at pos %02d: 0x%02X", inputPos, in_byte);
                #endif
                char ch = (char) in_byte;
                if (inputPos == 0 && ch != 0xFE) {
                    #ifdef CO2_DEBUG
                        logger.log("[Sensair S8] Discarding unexpected byte at pos 0.");
                    #endif
                    continue;
                }
                if (inputPos == 1 && ch != 0x04) {
                    #ifdef CO2_DEBUG
                        logger.log("[Sensair S8] Discarding unexpected byte at pos 1.");
                    #endif
                    inputPos = 0;
                    continue;
                }
                if (inputPos == 2 && ch != 0x02) {
                    #ifdef CO2_DEBUG
                        logger.log("[Sensair S8] Discarding unexpected byte at pos 2.");
                    #endif
                    inputPos = 0;
                    continue;
                }
                input[inputPos] = ch;
                inputPos++;
                if (inputPos > 6) {
                    break;
                }
            }

            // If there are 7 bytes - process the response.
            if (inputPos > 6) {
                #ifdef CO2_DEBUG
                    logger.log("[Sensair S8] Got %02d bytes, processing data.", inputPos);
                    for (int i = 0; i < inputPos; i++) {
                        logger.log("[Sensair S8] data[%02d] = 0x%02X", i, input[i]);
                    }
                #endif
                co2 = (input[3]<<8) + input[4];
                inputPos = 0;
                #ifdef CO2_DEBUG
                    logger.log("[Sensair S8] CO2 = %d", co2);
                #endif
            }
        }

        int getCO2() {
            return co2;
        }

        bool calibrate() {
            delay(200);
            Serial.flush();

            // 1. Clear acknowledgement register.
            uint8_t cmd1[] = {0xFE, 0x06, 0x00, 0x00, 0x00, 0x00, 0x9D, 0xC5};
            if (!sendAndValidate(cmd1, 8, cmd1, 8)) {
                logger.log("Failed to ack register.");
                return false;
            }

            // 2. Initiate calibration process.
            uint8_t cmd2[] = {0xFE, 0x06, 0x00, 0x01, 0x7C, 0x06, 0x6C, 0xC7};
            if (!sendAndValidate(cmd2, 8, cmd2, 8)) {
                logger.log("Failed to start calibration.");
                return false;
            }

            // 3. Wait for a single measurment to pass.
            delay (5000);

            // 4. Validate calibration has completed.
            uint8_t cmd3[] = {0xFE, 0x03, 0x00, 0x00, 0x00, 0x01, 0x90, 0x05};
            uint8_t resp3[] = {0xFE, 0x03, 0x00, 0x00, 0x20};
            if (!sendAndValidate(cmd3, 8, resp3, 5, 1000, true)) {
                logger.log("Failed to validate ack register.");
                return false;
            }

            logger.log("CO2 sensor racalibrated at 400ppm");
            return true;
        }
    private:
        unsigned long lastRead;
        int16_t co2;
        uint8_t input[16];
        int8_t inputPos;

        bool sendAndValidate(uint8_t* request,
                             uint8_t requestSize,
                             uint8_t* response,
                             uint8_t responseSize,
                             int16_t wait=1000,
                             bool andCompare=false) {
            Serial.write(request, requestSize);
            Serial.flush();

            uint8_t responseIndex = 0;
            while (responseIndex < responseSize && wait > 0) {
                while (Serial.available()) {
                    if (andCompare) {
                        if ((((uint8_t)Serial.read()) & response[responseIndex]) != response[responseIndex]) {
                            return false;
                        }
                    } else {
                        if (((uint8_t)Serial.read()) != response[responseIndex]) {
                            return false;
                        }
                    }
                    responseIndex++;
                }
            }

            return responseIndex == responseSize;
        } 
};
