#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class FeedController : public drogon::HttpController<FeedController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(FeedController::getFeed, "/feed", drogon::Get, AUTH_CHAIN);
    METHOD_LIST_END

    static void getFeed(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};