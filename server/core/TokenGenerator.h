#pragma once
#include <string>
#include <uuid/uuid.h>

class TokenGenerator {
public:
    static std::string generate() {
        uuid_t uuid;
        uuid_generate_random(uuid);

        char out[37];
        uuid_unparse(uuid, out);
        return std::string(out);
    }
};
