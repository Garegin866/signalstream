#pragma once
#include <string>
#include <drogon/utils/Utilities.h>

class PasswordHasher {
public:
    static std::string hash(const std::string &password) {
        return drogon::utils::md5(password);
    }

    static bool verify(const std::string &password, const std::string &hash) {
        return drogon::utils::md5(password) == hash;
    }
};
