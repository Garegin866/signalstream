#pragma once

#include <string>
#include <random>
#include <drogon/utils/Utilities.h>

class TokenGenerator {
public:
    static std::string generate() {
            return drogon::utils::getUuid();
    }

    static std::string secureRandom(size_t bytes) {
        static const char charset[] =
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz";

        static thread_local std::random_device rd;
        static thread_local std::mt19937_64 gen(rd());
        std::uniform_int_distribution<size_t> dist(0, sizeof(charset) - 2);

        std::string out;
        out.reserve(bytes);

        for (size_t i = 0; i < bytes; ++i) {
            out.push_back(charset[dist(gen)]);
        }

        return out;
    }
};
