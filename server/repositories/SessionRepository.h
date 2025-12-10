#pragma once
#include <drogon/orm/DbClient.h>
#include "../dto/AuthDTO.h"
#include "../core/Error.h"

class SessionRepository {
public:
    static void createSession(
            drogon::orm::DbClientPtr client,
            int userId,
            const std::string &token,
            std::function<void(const SessionDTO&, const AppError&)> cb
    );

    static void findByToken(
            drogon::orm::DbClientPtr client,
            const std::string& token,
            std::function<void(const std::optional<SessionDTO>&, const AppError&)> cb
    );

    static void deleteByToken(
            drogon::orm::DbClientPtr client,
            const std::string& token,
            std::function<void(bool, const AppError&)> cb
    );
};