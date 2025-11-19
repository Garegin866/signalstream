#pragma once
#include <string>
#include <argon2.h>
#include <random>

class PasswordHasher {
public:
    static std::string hash(const std::string &password) {
        const uint32_t t_cost = 3;              // iterations
        const uint32_t m_cost = 1 << 16;        // 64MB
        const uint32_t parallelism = 1;

        // ---- generate random 16-byte salt ----
        unsigned char salt[16];
        fillRandom(salt, sizeof(salt));

        // ---- encoded output buffer ----
        char hashbuf[256];  // plenty of space

        size_t hashlen = 32;  // output hash length (32 bytes recommended)

        int result = argon2id_hash_encoded(
                t_cost,
                m_cost,
                parallelism,
                password.data(),
                password.size(),
                salt,
                sizeof(salt),
                hashlen,
                hashbuf,
                sizeof(hashbuf)
        );

        if (result != ARGON2_OK)
            throw std::runtime_error("Argon2 hashing failed");

        return std::string(hashbuf);
    }

    static bool verify(const std::string &password, const std::string &encoded) {
        int result = argon2id_verify(
                encoded.c_str(),
                password.data(),
                password.size()
        );

        return result == ARGON2_OK;
    }

private:
    static void fillRandom(unsigned char *buf, size_t len) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(0, 255);

        for (size_t i = 0; i < len; i++)
            buf[i] = static_cast<unsigned char>(dis(gen));
    }
};
