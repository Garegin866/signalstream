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
};