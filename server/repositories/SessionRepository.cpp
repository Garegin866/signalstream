#include "SessionRepository.h"

void SessionRepository::createSession(
        drogon::orm::DbClientPtr client,
        int userId,
        const std::string &token,
        std::function<void(const SessionDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO sessions (user_id, token) VALUES ($1, $2) RETURNING id, user_id, token;",
            [cb](const drogon::orm::Result &r) {
                SessionDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.userId = r[0]["user_id"].as<int>();
                dto.token = r[0]["token"].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                cb({}, AppError{ErrorType::Database, "Database error"});
            },
            userId, token
    );
}


void SessionRepository::findByToken(
        drogon::orm::DbClientPtr client,
        const std::string &token,
        std::function<void(const std::optional<SessionDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, user_id, token FROM sessions WHERE token=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError{ErrorType::NotFound, "Session not found"});
                    return;
                }

                SessionDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.userId = r[0]["user_id"].as<int>();
                dto.token = r[0]["token"].as<std::string>();

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError{ErrorType::Database, "Database error"});
            },
            token
    );
}