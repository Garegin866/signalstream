#pragma once

#include <drogon/orm/DbClient.h>

#include "dto/AuthDTO.h"
#include "core/AppError.h"

class UserRepository {
public:
    static void createUser(
            const drogon::orm::DbClientPtr& client,
            const std::string &email,
            const std::string &passwordHash,
            const std::function<void(const UserDTO&, const AppError&)>& cb
    );

    static void findByEmail(
            const drogon::orm::DbClientPtr& client,
            const std::string &email,
            const std::function<void(const std::optional<UserDTO>&, const std::string &passwordHash, const AppError&)>& cb
    );

    static void findById(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::function<void(const std::optional<UserDTO>&, const AppError&)>& cb
    );

    static void listAllUsers(
            const drogon::orm::DbClientPtr& client,
            const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
    );

    static void listActiveEmails(
            const drogon::orm::DbClientPtr& client,
            const std::function<void(const std::vector<std::string>&, const AppError&)>& cb
    );

    static void updateRole(
            const drogon::orm::DbClientPtr& client,
            int userId,
            UserRole role,
            const std::function<void(const UserDTO&, const AppError&)>& cb
    );

    static void listModerators(
            const drogon::orm::DbClientPtr& client,
            const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
    );

    static void updatePassword(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::string& passwordHash,
            const std::function<void(const AppError&)>& cb
    );
};
