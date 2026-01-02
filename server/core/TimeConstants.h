#pragma once

#include <chrono>

namespace TimeConstants {

    // Password reset token lifetime
    inline constexpr std::chrono::minutes PASSWORD_RESET_TTL{20};

    // Session TTLs (future-proofing)
    inline constexpr std::chrono::hours SESSION_TTL{24};

    // Notification cleanup window (example)
    inline constexpr std::chrono::hours NOTIFICATION_TTL{168}; // 7 days

}