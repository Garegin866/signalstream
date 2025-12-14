#pragma once
#include <drogon/orm/DbClient.h>
#include "../dto/AuthDTO.h"
#include "../core/Error.h"

class UserRepository {
public:
    static void createUser(
            drogon::orm::DbClientPtr client,
            const std::string &email,
            const std::string &passwordHash,
            std::function<void(const UserDTO&, const AppError&)> cb
    );

    static void findByEmail(
            drogon::orm::DbClientPtr client,
            const std::string &email,
            std::function<void(const std::optional<UserDTO>&, const std::string &passwordHash, const AppError&)> cb
    );

    static void findById(
            drogon::orm::DbClientPtr client,
            int userId,
            std::function<void(const std::optional<UserDTO>&, const AppError&)> cb
    );

    static void listAllUsers(
            drogon::orm::DbClientPtr client,
            std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
    );

    static void updateRole(
            drogon::orm::DbClientPtr client,
            int userId,
            UserRole role,
            std::function<void(const UserDTO&, const AppError&)> cb
    );
};
