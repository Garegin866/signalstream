#pragma once

#include "dto/AuthDTO.h"
#include "core/AppError.h"

#include <functional>
#include <optional>
#include <drogon/orm/DbClient.h>

class AuthService {
public:
    explicit AuthService(drogon::orm::DbClientPtr client);

    void registerUser(
            const std::string &email,
            const std::string &password,
            const std::function<void(const UserDTO&, const AppError&)>& cb
    );

    void loginUser(
            const std::string &email,
            const std::string &password,
            const std::function<void(const SessionDTO&, const AppError&)>& cb
    );

    void logout(
            const std::string &token,
            const std::function<void(const AppError&)>& cb
    );

    void requestPasswordReset(
            const std::string& email,
            std::function<void(const AppError&)>&& cb
    );

    void resetPassword(
            const std::string& token,
            const std::string& newPassword,
            std::function<void(const AppError&)>&& cb
    );

private:
    drogon::orm::DbClientPtr client_;
};