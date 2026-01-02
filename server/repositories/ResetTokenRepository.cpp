#include "ResetTokenRepository.h"

static std::string toPgTimestamp(std::chrono::system_clock::time_point tp) {
    std::time_t t = std::chrono::system_clock::to_time_t(tp);
    std::tm tm{};
#if defined(_WIN32)
    gmtime_s(&tm, &t);
#else
    gmtime_r(&t, &tm);
#endif
    char buf[32];
    std::snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d",
                  tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday,
                  tm.tm_hour, tm.tm_min, tm.tm_sec);
    return std::string(buf);
}

void ResetTokenRepository::create(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::string& token,
        const std::chrono::system_clock::time_point& expiresAt,
        const std::function<void(const AppError&)>& cb
) {
    auto expiresStr = toPgTimestamp(expiresAt);

    client->execSqlAsync(
            "INSERT INTO reset_tokens (user_id, token, expires_at)"
            "VALUES ($1, $2, $3);",
            [cb](const drogon::orm::Result&) {
                cb(AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(AppError::Database("Failed to create reset token"));
            },
            userId,
            token,
            expiresStr
    );
}

void ResetTokenRepository::findValid(
        const drogon::orm::DbClientPtr& client,
        const std::string& token,
        const std::function<void(const std::optional<ResetTokenDTO>& dto, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT user_id, token "
            "FROM reset_tokens "
            "WHERE token=$1 "
            "AND used_at IS NULL "
            "AND expires_at > NOW();",
            [cb](const drogon::orm::Result& r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError{});
                    return;
                }

                ResetTokenDTO dto;
                dto.userId = r[0]["user_id"].as<int>();
                dto.token  = r[0]["token"].as<std::string>();

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError::Database("Failed to read reset token"));
            },
            token
    );
}

void ResetTokenRepository::markUsed(
        const drogon::orm::DbClientPtr& client,
        const std::string& token,
        const std::function<void(const AppError&)>& cb
) {
    client->execSqlAsync(
            "UPDATE reset_tokens "
            "SET used_at = NOW() "
            "WHERE token=$1;",
            [cb](const drogon::orm::Result&) {
                cb(AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(AppError::Database("Failed to mark reset token used"));
            },
            token
    );
}