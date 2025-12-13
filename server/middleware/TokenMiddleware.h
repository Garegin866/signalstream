#pragma once
#include <drogon/HttpFilter.h>

class TokenMiddleware : public drogon::HttpFilter<TokenMiddleware> {
public:
    void doFilter(
            const drogon::HttpRequestPtr& req,
            drogon::FilterCallback&& fcb,
            drogon::FilterChainCallback&& fccb
    ) override;
};