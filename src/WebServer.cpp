#include "AQMonitor.h"

char buffer[4096];

WebServer::WebServer(NetworkSettings* networkSettings, Logger* logger)
    :WebServerBase(networkSettings, logger, NULL) {
}

void WebServer::registerHandlers() {
    server->on("/", std::bind(&WebServer::handle_root, this));
    server->on("/settings", std::bind(&WebServer::handle_settings, this));
    server->on("/get", std::bind(&WebServer::handle_get, this));
}

void WebServer::handle_root() {
    server->sendHeader("Location","/settings");
    server->send(303);
}

void WebServer::handle_settings() {
    bool save = false;

    wifi.parse_config_params(this, save);
    dataCollector.parse_config_params(this, save);

    if (save) {
        settings.save();
    }

    char network_settings[strlen_P(NETWORK_CONFIG_PAGE) + 32];
    wifi.get_config_page(network_settings);

    char data_collector_settings[strlen_P(INFLUXDB_CONFIG_PAGE) + 96];
    dataCollector.get_config_page(data_collector_settings);


    sprintf_P(
        buffer,
        CONFIG_PAGE,
        network_settings,
        data_collector_settings);
    server->send(200, "text/html", buffer);
}

void WebServer::handle_get() {
    sprintf_P(buffer,
              GET_JSON,
              tempSensor.getTemperature(),
              tempSensor.getHumidity(),
              tempSensor.getPressure(),
              co2.getCO2());
    server->send(200, "application/json", buffer);
}