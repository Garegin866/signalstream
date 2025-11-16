#pragma once

#include <drogon/HttpSimpleController.h>

class HealthController : public drogon::HttpSimpleController<HealthController> {
public:
    void asyncHandleHttpRequest(
            const drogon::HttpRequestPtr& req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    ) override;

    PATH_LIST_BEGIN
    PATH_ADD("/health", drogon::Get); // GET /health
    PATH_LIST_END
};
