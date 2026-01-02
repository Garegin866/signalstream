#include "SMTPEmailService.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

#include "core/Error.h"
#include "core/ErrorLogger.h"
#include "core/EmailUtils.h"

using namespace drogon;

namespace {

    bool isValidEmail(const std::string& email) {
        return email.find('@') != std::string::npos;
    }

} // namespace

void SMTPEmailService::sendEmail(
        const std::string& to,
        const std::string& subject,
        const std::string& body,
        std::function<void(const AppError&)> cb
) {
    const auto& cfg = app().getCustomConfig();

    if (!cfg.isMember("email") || !cfg["email"]["enabled"].asBool()) {
        cb(AppError::External("Email service disabled"));
        return;
    }

    if (!isValidEmail(to)) {
        cb(AppError::Validation("Invalid email address"));
        return;
    }

    const auto& smtp = cfg["email"]["smtp"];

    try {
        std::string host     = smtp["host"].asString();
        int         port     = smtp["port"].asInt();
        std::string username = smtp["username"].asString();
        std::string password = smtp["password"].asString();
        std::string from     = smtp["from"].asString();
        bool        useTls   = smtp["use_tls"].asBool();

        ss::utils::sendEmail(
                host,
                port,
                useTls,
                username,
                password,
                from,
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
    } catch (const std::exception& e) {
        cb(AppError::External("Email service failure"));
    }
}