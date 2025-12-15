#include "SessionRepository.h"

#include "core/Constants.h"

void SessionRepository::createSession(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::string &token,
        const std::function<void(const SessionDTO&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "INSERT INTO sessions (user_id, token) VALUES ($1, $2) RETURNING id, user_id, token;",
            [cb](const drogon::orm::Result &r) {
                SessionDTO dto;
                dto.id = r[0][Const::COL_ID].as<int>();
                dto.userId = r[0][Const::COL_USER_ID].as<int>();
                dto.token = r[0][Const::COL_TOKEN].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                cb({}, AppError::Database("Database error"));
            },
            userId, token
    );
}


void SessionRepository::findByToken(
        const drogon::orm::DbClientPtr& client,
        const std::string &token,
        const std::function<void(const std::optional<SessionDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, user_id, token FROM sessions WHERE token=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb({}, AppError::NotFound("Session not found"));
                    return;
                }

                SessionDTO dto;
                dto.id = r[0][Const::COL_ID].as<int>();
                dto.userId = r[0][Const::COL_USER_ID].as<int>();
                dto.token = r[0][Const::COL_TOKEN].as<std::string>();

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            token
    );
}

void SessionRepository::deleteByToken(
        const drogon::orm::DbClientPtr& client,
        const std::string &token,
        const std::function<void(bool, const AppError&)>& cb
) {
    client->execSqlAsync(
            "DELETE FROM sessions WHERE token=$1;",
            [cb](const drogon::orm::Result &r) {
                cb(true, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(false, AppError::Database("Database error"));
            },
            token
    );
}