#pragma once

#include <drogon/drogon.h>

namespace Profiler {

    inline void log(const std::string& scope, long ms) {
        if (!drogon::app().getCustomConfig()["profiling"]["enabled"].asBool()) {
            return;
        }

        LOG_INFO << "[perf]"
                 << " scope=" << scope
                 << " duration_ms=" << ms;
    }

}