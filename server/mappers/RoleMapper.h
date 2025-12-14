#pragma once

#include "dto/AuthDTO.h"

#include <stdexcept>

inline std::string toString(UserRole role) {
    switch (role) {
        case UserRole::User:  return "user";
        case UserRole::Admin: return "admin";
        default: return "unknown";
    }
}

inline UserRole fromString(const std::string &s) {
    if (s == "user")  return UserRole::User;
    if (s == "admin") return UserRole::Admin;
    throw std::runtime_error("Invalid role string: " + s);
}