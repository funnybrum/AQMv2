#pragma once

#include "AQMonitor.h"
#include "WebServerBase.h"
#include "WebPages.h"

class WebServer: public WebServerBase {
    public:
        WebServer(NetworkSettings* networkSettings, Logger* logger);
        void registerHandlers();
        void handle_stats();
        void handle_root();
        void handle_get();
        void handle_settings();
};
