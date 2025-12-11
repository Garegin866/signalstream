#pragma once

#include <drogon/HttpController.h>

class FeedController : public drogon::HttpController<FeedController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(FeedController::getFeed, "/feed", drogon::Get);
    METHOD_LIST_END

    void getFeed(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};