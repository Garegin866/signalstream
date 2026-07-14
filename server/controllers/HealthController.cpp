#include "HealthController.h"

#include "core/Response.h"

#include <json/json.h>

void HealthController::health(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb
) {
    Json::Value body;
    body["status"] = "ok";

    cb(jsonOK(body));
}