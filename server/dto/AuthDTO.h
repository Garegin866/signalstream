#pragma once
#include <string>

struct UserDTO {
    int id = 0;
    std::string email;
    std::string role = "user";

    [[nodiscard]] bool isAdmin() const {
        return role == "admin";
    }
};

struct SessionDTO {
    int id;
    int userId;
    std::string token;
};