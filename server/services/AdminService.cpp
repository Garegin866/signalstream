#include "AdminService.h"

#include "repositories/UserRepository.h"
#include "services/RoleService.h"
#include "services/EmailService.h"

#include "core/Version.h"
#include "core/Uptime.h"

#include <drogon/drogon.h>

void AdminService::listUsers(
        const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();
    UserRepository::listAllUsers(client, cb);
}

void AdminService::setRole(int actorId, int targetUserId, UserRole newRole,
        const std::function<void(const UserDTO&, const AppError&)>& cb
) {
    if (!RoleService::isValidRole(newRole)) {
        cb({}, AppError::Validation("Invalid role"));
        return;
    }

    if (actorId == targetUserId && newRole != UserRole::Admin) {
        cb({}, AppError::Forbidden("Admin cannot remove their own admin role"));
        return;
    }

    auto client = drogon::app().getDbClient();
    UserRepository::updateRole(client, targetUserId, newRole, cb);
}

void AdminService::listModerators(
        const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    UserRepository::listModerators(client, cb);
}

void AdminService::health(
        const std::function<void(const HealthDTO&)>& cb
) {
    auto client = drogon::app().getDbClient();

    client->execSqlAsync(
            "SELECT 1;",
            [cb](const drogon::orm::Result&) {
                HealthDTO dto;
                dto.status = "ok";
                dto.db = "ok";
                dto.uptimeSec = Uptime::seconds();
                dto.timestamp = trantor::Date::now().toFormattedString(true);
                dto.version = APP_VERSION;
                cb(dto);
            },
            [cb](const std::exception_ptr&) {
                HealthDTO dto;
                dto.status = "degraded";
                dto.db = "down";
                dto.uptimeSec = Uptime::seconds();
                dto.timestamp = trantor::Date::now().toFormattedString(true);
                dto.version = APP_VERSION;
                cb(dto);
            }
    );
}

void AdminService::broadcastEmail(
        const std::string& subject,
        const std::string& message,
        const std::function<void(const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    UserRepository::listActiveEmails(
            client,
            [subject, message, cb](const std::vector<std::string>& emails, const AppError& err) {
                if (err.hasError()) {
                    cb(err);
                    return;
                }

                EmailService::broadcast(
                        emails,
                        subject,
                        message,
                        cb
                );
            }
    );
}