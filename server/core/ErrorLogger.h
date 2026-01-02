#pragma once

#include "Error.h"

#include <drogon/HttpRequest.h>

class ErrorLogger {
public:
    static void logAppError(
            const AppError& err,
            const drogon::HttpRequestPtr& req
    );
};