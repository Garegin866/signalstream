#pragma once

#include "dto/AuthDTO.h"

#include <stdexcept>

inline std::string toString(UserRole role) {
    switch (role) {
        case UserRole::User:  return "user";
        case UserRole::Moderator:  return "moderator";
        case UserRole::Admin: return "admin";
        default: return "invalid";
    }
}

inline UserRole fromString(const std::string &s) {
    if (s == "user")  return UserRole::User;
    if (s == "moderator") return UserRole::Moderator;
    if (s == "admin") return UserRole::Admin;
    return UserRole::Invalid;
}