#pragma once

#include "core/AppError.h"

#include <drogon/HttpRequest.h>

class ErrorLogger {
public:
    static void logAppError(
            const AppError& err,
            const drogon::HttpRequestPtr& req
    );
};