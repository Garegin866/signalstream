#pragma once

#include "dto/AuthDTO.h"
#include "core/AppError.h"

#include <functional>
#include <optional>

class AuthService {
public:
    static void registerUser(
            const std::string &email,
            const std::string &password,
            const std::function<void(const UserDTO&, const AppError&)>& cb
    );

    static void loginUser(
            const std::string &email,
            const std::string &password,
            const std::function<void(const SessionDTO&, const AppError&)>& cb
    );

    static void logout(
            const std::string &token,
            const std::function<void(const AppError&)>& cb
    );

    static void requestPasswordReset(
            const std::string& email,
            std::function<void(const AppError&)>&& cb
    );

    static void resetPassword(
            const std::string& token,
            const std::string& newPassword,
            std::function<void(const AppError&)>&& cb
    );
};