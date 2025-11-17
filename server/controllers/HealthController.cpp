#include "HealthController.h"
#include <drogon/drogon.h>

using namespace drogon;

void HealthController::asyncHandleHttpRequest(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    auto client = drogon::app().getDbClient();

    client->execSqlAsync(
            "SELECT 1;",
            [callback](const orm::Result &r) {
                Json::Value json;
                json["status"] = "ok";
                json["db"] = "ok";

                auto resp = HttpResponse::newHttpJsonResponse(json);
                callback(resp);
            },
            [callback](const std::exception_ptr &eptr) {
                Json::Value json;
                json["status"] = "error";

                try {
                    if (eptr) {
                        std::rethrow_exception(eptr);
                    }
                } catch (const std::exception &e) {
                    json["db_error"] = e.what();
                }

                auto resp = HttpResponse::newHttpJsonResponse(json);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            }
    );
}
