#pragma once

#include "AQMonitor.h"

// #define PM_DEBUG
#ifndef PM_DEBUG
    #pragma GCC diagnostic ignored "-Wunused-variable"
#endif

#include <SoftwareSerial.h>

/**
 * Plantower PMSx003 driver. Pull the PM values from the sensor on predefined interval.
 * The sensor must be connected as follows:
 * PMSx003 TX <---> GPIO2
 * PMSx003 RX <---> GPIO0
 */

class PlantowerPMSx003 {
    public:
        PlantowerPMSx003(uint8_t pmsTxPin = 2, uint8_t pmsRxPin = 0) {
            this->mySerial = new SoftwareSerial(pmsTxPin, pmsRxPin);
        }

        void begin() {
            this->mySerial->begin(9600);
            #ifdef PM_DEBUG
            	logger.log("[PMSx003] Serial started at 9600bps");
            #endif
            inputPos = 0;
        }

        void loop() {
            // Process any available bytes on the Serial input.
            while (mySerial->available() > 0) {
                int in_byte = mySerial->read();
                #ifdef PM_DEBUG
                    logger.log("[PMSx003] Got char at pos %02d: 0x%02X", inputPos, in_byte);
                #endif
                char ch = (char) in_byte;
                if (inputPos == 0 && ch != 0x42) {
                    #ifdef PM_DEBUG
                        logger.log("[PMSx003] Discarding unexpected byte at pos 0.");
                    #endif
                    continue;
                }
                if (inputPos == 1 && ch != 0x4d) {
                    #ifdef PM_DEBUG
                        logger.log("[PMSx003] Discarding unexpected byte at pos 1.");
                    #endif
                    inputPos = 0;
                    continue;
                }
                input[inputPos] = ch;
                inputPos++;
                if (inputPos > 32) {
                    break;
                }
            }

            // If there are 32 bytes - process the response.
            if (inputPos > 32) {
                #ifdef PM_DEBUG
                    logger.log("[PMSx003] Got %02d bytes, processing data.", inputPos);
                    for (int i = 0; i < inputPos; i++) {
                        logger.log("[PMSx003] data[%02d] = 0x%02X", i, input[i]);
                    }
                #endif
                pm01 = (input[4]<<8) + input[5];
                pm02 = (input[6]<<8) + input[7];
                pm10 = (input[8]<<8) + input[9];
                inputPos = 0;
                #ifdef PM_DEBUG
                    logger.log("[PMSx003] PM = %d, %d, %d", pm01, pm02, pm10);
                #endif
            }
        }

        int getPM01() {
            return pm01;
        }

        int getPM02() {
            return pm02;
        }

        int getPM10() {
            return pm10;
        }

        void sleap() {
            // TODO
        }

        void awake() {
            // TODO
        }
    private:
        int16_t pm01, pm02, pm10;
        uint8_t input[32];
        int8_t inputPos;

        SoftwareSerial* mySerial;
};
