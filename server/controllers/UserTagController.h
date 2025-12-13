#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class UserTagController : public drogon::HttpController<UserTagController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(UserTagController::attach, "/user/tags", drogon::Post, AUTH_CHAIN);
    ADD_METHOD_TO(UserTagController::list, "/user/tags", drogon::Get, AUTH_CHAIN);
    METHOD_LIST_END

    static void attach(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );

    static void list(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};