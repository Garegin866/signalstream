#pragma once

#include <drogon/HttpTypes.h>

#include "Error.h"

static inline std::string normalizeMsg(std::string msg, std::string fallback) {
    auto is_ws = [](unsigned char c){ return std::isspace(c); };
    bool onlyWs = !msg.empty() && std::all_of(msg.begin(), msg.end(), is_ws);
    if (msg.empty() || onlyWs) return std::move(fallback);
    return msg;
}

struct AppError {
    ErrorType type = ErrorType::None;
    std::string message;

    AppError() = default;

    AppError(ErrorType t, std::string msg)
            : type(t), message(std::move(msg)) {}

    [[nodiscard]] bool hasError() const {
        return type != ErrorType::None;
    }

    // ---- Factory methods ----
    static AppError Validation(std::string msg) {
        return AppError{ErrorType::Validation, normalizeMsg(std::move(msg), "Validation error")};
    }

    static AppError NotFound(std::string msg) {
        return AppError{ErrorType::NotFound, normalizeMsg(std::move(msg), "Not found")};
    }

    static AppError Duplicate(std::string msg) {
        return AppError{ErrorType::Duplicate, normalizeMsg(std::move(msg), "Duplicate resource")};
    }

    static AppError Unauthorized(std::string msg) {
        return AppError{ErrorType::Unauthorized, normalizeMsg(std::move(msg), "Unauthorized")};
    }

    static AppError Forbidden(std::string msg) {
        return AppError{ErrorType::Forbidden, normalizeMsg(std::move(msg), "Forbidden")};
    }

    static AppError Database(std::string msg) {
        return AppError{ErrorType::Database, normalizeMsg(std::move(msg), "Database error")};
    }

    static AppError Internal(std::string msg) {
        return AppError{ErrorType::Internal, normalizeMsg(std::move(msg), "Internal server error")};
    }

    static AppError External(std::string msg) {
        return AppError{ErrorType::External, normalizeMsg(std::move(msg), "External service error")};
    }
};

inline drogon::HttpStatusCode toHttpStatus(const AppError& err) {
    switch (err.type) {
        case ErrorType::Validation:
            return drogon::k400BadRequest;
        case ErrorType::Unauthorized:
            return drogon::k401Unauthorized;
        case ErrorType::Forbidden:
            return drogon::k403Forbidden;
        case ErrorType::NotFound:
            return drogon::k404NotFound;
        case ErrorType::Duplicate:
            return drogon::k409Conflict;
        case ErrorType::Database:
        case ErrorType::Internal:
            return drogon::k500InternalServerError;
        case ErrorType::External:
            return drogon::k503ServiceUnavailable;
        case ErrorType::None:
        default:
            return drogon::k200OK;
    }
}
