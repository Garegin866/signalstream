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