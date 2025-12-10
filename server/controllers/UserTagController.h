#pragma once

#include <drogon/HttpController.h>

class UserTagController : public drogon::HttpController<UserTagController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserTagController::attach, "/user/tags", drogon::Post);
    METHOD_LIST_END

    void attach(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};