#pragma once

#include "EmailService.h"

class SMTPEmailService final : public EmailService {
public:
    void sendEmail(
            const std::string& to,
            const std::string& subject,
            const std::string& body,
            std::function<void(const AppError&)> cb
    ) override;
};