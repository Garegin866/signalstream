#pragma once
#include <string>

struct UserDTO {
    int id;
    std::string email;
};

struct SessionDTO {
    int id;
    int userId;
    std::string token;
};