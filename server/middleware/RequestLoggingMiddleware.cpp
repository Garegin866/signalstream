#include "RequestLoggingMiddleware.h"

#include <drogon/drogon.h>
#include <chrono>

#include "core/Constants.h"

using namespace drogon;

void RequestLoggingMiddleware::invoke(
        const HttpRequestPtr& req,
        MiddlewareNextCallback&& next,
        MiddlewareCallback&& callback
) {
    const auto start = std::chrono::steady_clock::now();

    next([req, callback = std::move(callback), start](const HttpResponsePtr& resp) mutable {
        if (!app().getCustomConfig()["logging"]["request"]["enabled"].asBool()) {
            callback(resp);
            return;
        }

        const auto end = std::chrono::steady_clock::now();
        const auto duration =
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

        int status = resp ? resp->statusCode() : -1;

        std::string userId = "-";
        if (auto attrs = req->attributes()) {
            if (attrs->find(Const::ATTR_USER_ID)) {
                userId = std::to_string(attrs->get<int>(Const::ATTR_USER_ID));
            }
        }

        LOG_INFO
            << "request "
            << "method=" << req->methodString()
            << " path=" << req->path()
            << " status=" << status
            << " duration_ms=" << duration
            << " user_id=" << userId;

        callback(resp);
    });
}