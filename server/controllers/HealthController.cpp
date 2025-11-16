#include "HealthController.h"

#include <drogon/drogon.h>

using namespace drogon;

void HealthController::asyncHandleHttpRequest(
        const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback
) {
    Json::Value json;
    json["status"] = "ok";

    auto resp = HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(k200OK);
    callback(resp);
}
