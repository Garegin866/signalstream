#pragma once

#include "core/Error.h"

#include <string>
#include <functional>

class EmailService {
public:
    static void sendPasswordReset(
            const std::string& to,
            const std::string& token,
            std::function<void(const AppError&)> cb
    );
};