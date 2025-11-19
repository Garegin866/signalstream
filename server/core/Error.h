#pragma once
#include <string>

enum class ErrorType {
    None,
    Validation,
    Duplicate,
    Database,
    Unauthorized,
    Unknown
};

struct AppError {
    ErrorType type = ErrorType::None;
    std::string message;
};
