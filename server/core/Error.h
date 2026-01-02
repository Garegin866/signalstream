#pragma once
#include <string>
#include <drogon/HttpTypes.h>

enum class ErrorType {
    None,

    // 400
    Validation,
    MissingField,
    InvalidFormat,
    BusinessRule,

    // 401 / 403
    Unauthorized,
    Forbidden,

    // 404
    NotFound,

    // 409
    Duplicate,

    // 500
    Database,
    Internal,

    // 503 - Service Unavailable
    External
};

struct AppError {
    ErrorType type = ErrorType::None;
    std::string message;

    AppError() = default;

    AppError(ErrorType t, std::string msg)
            : type(t), message(std::move(msg)) {}

    bool hasError() const {
        return type != ErrorType::None;
    }

    // ---- Factory methods ----
    static AppError Validation(std::string msg) {
        return AppError{ErrorType::Validation, std::move(msg)};
    }

    static AppError NotFound(std::string msg) {
        return AppError{ErrorType::NotFound, std::move(msg)};
    }

    static AppError Duplicate(std::string msg) {
        return AppError{ErrorType::Duplicate, std::move(msg)};
    }

    static AppError Unauthorized(std::string msg) {
        return AppError{ErrorType::Unauthorized, std::move(msg)};
    }

    static AppError Forbidden(std::string msg) {
        return AppError{ErrorType::Forbidden, std::move(msg)};
    }

    static AppError Database(std::string msg) {
        return AppError{ErrorType::Database, std::move(msg)};
    }

    static AppError Internal(std::string msg) {
        return AppError{ErrorType::Internal, std::move(msg)};
    }

    static AppError External(std::string msg) {
        return AppError{ErrorType::External, std::move(msg)};
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
            return drogon::k500InternalServerError;
        case ErrorType::Internal:
            return drogon::k500InternalServerError;
        case ErrorType::External:
            return drogon::k503ServiceUnavailable;
        case ErrorType::None:
        default:
            return drogon::k200OK;
    }
}
