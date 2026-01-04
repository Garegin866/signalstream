#pragma once

#include <string>

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