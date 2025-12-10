#pragma once

#include <drogon/HttpController.h>

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser, "/auth/register", drogon::Post);
    ADD_METHOD_TO(AuthController::loginUser, "/auth/login", drogon::Post);
    ADD_METHOD_TO(AuthController::me, "/auth/me", drogon::Get);
    ADD_METHOD_TO(AuthController::logout, "/auth/logout", drogon::Post);
    METHOD_LIST_END

    static void registerUser(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );

    static void loginUser(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );

    static void me(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    static void logout(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );
};
