#pragma once

#include "core/Error.h"

#include <string>
#include <functional>

class EmailService {
public:
    virtual ~EmailService() = default;

    virtual void sendEmail(
            const std::string& to,
            const std::string& subject,
            const std::string& body,
            std::function<void(const AppError&)> cb
    ) = 0;
};