#include "AdminService.h"

#include "repositories/UserRepository.h"
#include "services/RoleService.h"

#include <drogon/drogon.h>

void AdminService::listUsers(
        std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();
    UserRepository::listAllUsers(client, cb);
}

void AdminService::setRole(int actorId, int targetUserId, UserRole newRole,
        std::function<void(const UserDTO&, const AppError&)> cb
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
        std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    UserRepository::listModerators(client, cb);
}