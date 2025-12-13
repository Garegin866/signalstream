#pragma once
#include <functional>
#include <optional>
#include "../dto/AuthDTO.h"
#include "../core/Error.h"

class AuthService {
public:
    static void registerUser(
            const std::string &email,
            const std::string &password,
            std::function<void(const UserDTO&, const AppError&)> cb
    );

    static void loginUser(
            const std::string &email,
            const std::string &password,
            std::function<void(const SessionDTO&, const AppError&)> cb
    );

    static void logout(
            const std::string &token,
            std::function<void(const AppError&)> cb
    );
};