#pragma once

#include "core/AppError.h"

#include <drogon/drogon.h>

struct ResetTokenDTO {
    int userId;
    std::string token;
};

class ResetTokenRepository {
public:
    static void create(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::string& token,
            const std::chrono::system_clock::time_point& expiresAt,
            const std::function<void(const AppError&)>& cb
    );

    static void findValid(
            const drogon::orm::DbClientPtr& client,
            const std::string& token,
            const std::function<void(const std::optional<ResetTokenDTO>& dto, const AppError&)>& cb
    );

    static void markUsed(
            const drogon::orm::DbClientPtr& client,
            const std::string& token,
            const std::function<void(const AppError&)>& cb
    );
};