#pragma once

#include "core/AppError.h"

#include <string>
#include <functional>

class EmailService {
public:
    static void sendPasswordReset(
            const std::string& to,
            const std::string& token,
            std::function<void(const AppError&)> cb
    );

    static void broadcast(
            const std::vector<std::string>& recipients,
            const std::string& subject,
            const std::string& body,
            std::function<void(const AppError&)> cb
    );
};