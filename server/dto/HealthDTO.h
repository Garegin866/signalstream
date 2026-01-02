#pragma once

#include <string>

struct HealthDTO {
    std::string status;   // ok | degraded
    std::string db;       // ok | down
    int64_t uptimeSec;
    std::string timestamp;
    std::string version;
};