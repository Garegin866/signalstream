#include "AdminService.h"
#include "repositories/UserRepository.h"

#include <drogon/drogon.h>

void AdminService::listUsers(
        std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();
    UserRepository::listAllUsers(client, cb);
}