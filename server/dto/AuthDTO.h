#pragma once
#include <string>

enum class UserRole {
    User,
    Admin,
};

struct UserDTO {
    int id = 0;
    std::string email;
    UserRole role = UserRole::User;

    [[nodiscard]] bool isAdmin() const {
        return role == UserRole::Admin;
    }
};

struct SessionDTO {
    int id;
    int userId;
    std::string token;
};