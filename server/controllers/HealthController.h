#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class HealthController : public drogon::HttpController<HealthController> {
public:
    METHOD_LIST_BEGIN
        METHOD_ADD(HealthController::health, "/health", drogon::Get, PUBLIC);
    METHOD_LIST_END

    void health(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};