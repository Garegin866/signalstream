#pragma once
#include <chrono>

namespace Uptime {
    inline const auto startTime = std::chrono::steady_clock::now();

    inline std::int64_t seconds() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                std::chrono::steady_clock::now() - startTime
        ).count();
    }
}