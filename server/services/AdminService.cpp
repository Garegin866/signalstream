#include "AdminService.h"
#include "repositories/UserRepository.h"

#include <drogon/drogon.h>

void AdminService::listUsers(
        std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();
    UserRepository::listAllUsers(client, cb);
}

void AdminService::setRole(
        int targetUserId,
        UserRole newRole,
        int actingUserId,
        std::function<void(const UserDTO&, const AppError&)> cb
) {
    // 1. Prevent self-demotion (optional)
    if (targetUserId == actingUserId) {
        cb({}, AppError::Forbidden("You cannot change your own role"));
        return;
    }

    auto client = drogon::app().getDbClient();

    // 2. Validate target user exists first
    UserRepository::findById(
            client,
            targetUserId,
            [client, newRole, cb](const std::optional<UserDTO>& user, const AppError& err) {
                if (err.hasError()) {
                    cb({}, err);
                    return;
                }
                if (!user) {
                    cb({}, AppError::NotFound("Target user not found"));
                    return;
                }

                // 3. Update role
                UserRepository::updateRole(
                        client,
                        user->id,
                        newRole,
                        [cb](const UserDTO& updated, const AppError& err2) {
                            cb(updated, err2);
                        }
                );
            }
    );
}

void AdminService::listModerators(
        std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    UserRepository::listModerators(client, cb);
}