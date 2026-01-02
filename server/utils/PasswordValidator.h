#pragma once

#include <string>

class PasswordValidator {
public:
    static bool isStrong(const std::string& password) {
        if (password.size() < 8)
            return false;

        bool hasUpper = false;
        bool hasLower = false;
        bool hasDigit = false;

        for (char c : password) {
            if (std::isupper(static_cast<unsigned char>(c))) hasUpper = true;
            else if (std::islower(static_cast<unsigned char>(c))) hasLower = true;
            else if (std::isdigit(static_cast<unsigned char>(c))) hasDigit = true;
        }

        return hasUpper && hasLower && hasDigit;
    }
};


