#pragma once
#include <string>
#include <drogon/utils/Utilities.h>

class TokenGenerator {
public:
    static std::string generate() {
            return drogon::utils::getUuid();
    }
};
