#pragma once

#include "AQMonitor.h"

#define CO2_READ_INTERVAL 10000
// #define CO2_DEBUG

/**
 * Very basic Sensair S8 driver. Pull the CO2 values from the sensor based on predefined interval.
 * The sensor must be connected on the hardware serial pins of the microcontroller.
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

        double loop() {
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
    private:
        unsigned long lastRead;
        int16_t co2;
        uint8_t input[16];
        int8_t inputPos;

};
