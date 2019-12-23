# AQM v2

Second version of the Air Quality Monitor. First one is [here](https://github.com/funnybrum/AQM), but the BME680 happened to produce not so meaningful results. This version uses sensors that measure absolute values:
1) CO2 sensor (Senasair S8).
2) BME280 for temperature/humidity/pressure.
3) PMSA003 for PM10 and PM2.5 measurments.

Still work in progress. The PCB is available at [EasyEDA](https://easyeda.com/funnybrum/iaq-board-v3-co2_copy). It fit nicely in out of the box [$2 enclosure](./docs/sb.jpg). Search for AK-N-41 on Aliexpress.

## Over-the-air programming

The firmware supports OTA update. Below curl command executed from the project root will perform the update:
> curl -F "image=@.pio/build/esp8285/firmware.bin" aq-monitor/update

## InfluxDB integration

The sensor can be configured for regular pushes to an InfluxDB (must be a DB that is not password protected). If the InfluxDB integration is enabled the sensor will turn off the WiFi and keep running by collecting data in the background. Once the telemtry data buffers are full or the time for pushing the data has come - the sensor will turn on the WiFi, push the data and turn it back off.

The WiFi will stay on until the first data push cycle has been completed. In this time window the sensor can be reconfigured (open the http://sensor_address/ to get to the settings page). 

## REST interface

The firmware provides a REST API to ease the usage. There are several endpoints that should be mentioned:

* http://{address}/get - get the current sensor readings as JSON document.
* http://{address}/logs - get the logs (for debugging purposes).
* http://{address}/settings - a form that is used to configure the sensor. Details like WiFi SSID/password, InfluxDB integration and offsets can be configured here.

## Building the project

The project uses a common set of tools that are availabe in another repo - https://github.com/funnybrum/esp8266-base. Clone the esp8266-base repo in the lib folder:

```
cd lib
git clone https://github.com/funnybrum/esp8266-base.git
```

After that try building the project.
