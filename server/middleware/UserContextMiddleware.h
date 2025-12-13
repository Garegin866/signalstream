#pragma once
#include <drogon/HttpFilter.h>

class UserContextMiddleware : public drogon::HttpFilter<UserContextMiddleware> {
public:
    void doFilter(
            const drogon::HttpRequestPtr& req,
            drogon::FilterCallback&& fcb,
            drogon::FilterChainCallback&& fccb
    ) override;
};