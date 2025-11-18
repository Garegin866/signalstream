#pragma once
#include <drogon/HttpController.h>

class AuthController : public drogon::HttpController<AuthController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(AuthController::registerUser, "/auth/register", drogon::Post);
    ADD_METHOD_TO(AuthController::loginUser, "/auth/login", drogon::Post);
    METHOD_LIST_END

    void registerUser(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );

    void loginUser(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );
};
