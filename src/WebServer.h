#pragma once

#include "AQMonitor.h"
#include "WebServerBase.h"
#include "WebPages.h"

class WebServer: public WebServerBase {
    public:
        WebServer(Logger* logger, NetworkSettings* networkSettings);
        void registerHandlers();
        void handle_stats();
        void handle_root();
        void handle_get();
        void handle_settings();
};
