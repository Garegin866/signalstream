#pragma once

#include "AuthDTO.h"

inline std::string toString(UserRole role) {
    switch (role) {
        case UserRole::Admin: return "admin";
        case UserRole::User:  return "user";
    }
    return "user";
}

inline UserRole fromString(const std::string& s) {
    if (s == "admin") return UserRole::Admin;
    return UserRole::User;
}