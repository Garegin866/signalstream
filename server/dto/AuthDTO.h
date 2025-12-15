#pragma once
#include <string>

enum class UserRole {
    User,
    Moderator,
    Admin,
    Invalid
};

inline int roleRank(UserRole r) {
    switch (r) {
        case UserRole::User:      return 1;
        case UserRole::Moderator: return 2;
        case UserRole::Admin:     return 3;
        case UserRole::Invalid:
            break;
    }
    return 0;
}

#define ROLE_AT_LEAST(user, required) (roleRank(user.role) >= roleRank(required))

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