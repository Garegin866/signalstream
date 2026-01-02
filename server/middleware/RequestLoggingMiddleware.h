#pragma once

#include <drogon/HttpMiddleware.h>

class RequestLoggingMiddleware : public drogon::HttpMiddleware<RequestLoggingMiddleware> {
public:
    void invoke(
            const drogon::HttpRequestPtr& req,
            drogon::MiddlewareNextCallback&& next,
            drogon::MiddlewareCallback&& callback
    ) override;
};