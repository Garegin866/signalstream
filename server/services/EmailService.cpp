#include "EmailService.h"

#include "utils/SmtpClient.h"
#include <drogon/drogon.h>

static utils::SmtpConfig loadSmtpConfig() {
    const auto& cfgJson = drogon::app().getCustomConfig()["email"];

    utils::SmtpConfig cfg;
    cfg.host     = cfgJson["host"].asString();
    cfg.port     = cfgJson["port"].asInt();
    cfg.useTls   = cfgJson["tls"].asBool();
    cfg.username = cfgJson["username"].asString();
    cfg.password = cfgJson["password"].asString();
    cfg.from     = cfgJson["from"].asString();
    return cfg;
}

void EmailService::sendPasswordReset(
        const std::string& to,
        const std::string& token,
        std::function<void(const AppError&)> cb
) {
    utils::SmtpConfig cfg = loadSmtpConfig();

    std::string subject = "Reset your SignalStream password";
    std::string body =
            "You requested a password reset.\n\n"
            "Use this token to reset your password:\n\n" +
            token +
            "\n\nIf you did not request this, ignore this email.";

    utils::SmtpClient::send(
            cfg,
            {to},
            subject,
            body,
            [cb](bool success, const std::string& error) {
                if (!success) {
                    cb(AppError::External("Email delivery failed"));
                    return;
                }
                cb(AppError{});
            }
    );
}

void EmailService::broadcast(
        const std::vector<std::string>& recipients,
        const std::string& subject,
        const std::string& body,
        std::function<void(const AppError&)> cb
) {
    utils::SmtpConfig cfg = loadSmtpConfig();

    if (recipients.empty()) {
        cb(AppError::Validation("Recipient list is empty"));
        return;
    }

    utils::SmtpClient::send(
            cfg,
            recipients,
            subject,
            body,
            [cb](bool success, const std::string& error) {
                if (!success) {
                    cb(AppError::External("Email delivery failed"));
                    return;
                }
                cb(AppError{});
            }
    );
}