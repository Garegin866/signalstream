#pragma once
#include <drogon/orm/DbClient.h>
#include "../dto/AuthDTO.h"
#include "../core/Error.h"

class SessionRepository {
public:
    static void createSession(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::string &token,
            const std::function<void(const SessionDTO&, const AppError&)>& cb
    );

    static void findByToken(
            const drogon::orm::DbClientPtr& client,
            const std::string& token,
            const std::function<void(const std::optional<SessionDTO>&, const AppError&)>& cb
    );

    static void deleteByToken(
            const drogon::orm::DbClientPtr& client,
            const std::string& token,
            const std::function<void(bool, const AppError&)>& cb
    );

    static void deleteByUserId(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::function<void(const AppError&)>& cb
    );
};