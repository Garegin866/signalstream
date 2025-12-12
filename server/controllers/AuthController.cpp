#include "AuthController.h"

#include <drogon/drogon.h>

#include "services/AuthService.h"
#include "core/Response.h"

using namespace drogon;

void AuthController::registerUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(jsonError(400, "Invalid body"));
        return;
    }

    auto email = (*json)["email"].asString();
    auto password = (*json)["password"].asString();

    AuthService::registerUser(
            email,
            password,
            [callback](const UserDTO &user, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body;
                body["id"] = user.id;
                body["email"] = user.email;

                callback(jsonCreated(body));
            }
    );
}

void AuthController::loginUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(jsonError(400, "Invalid body"));
        return;
    }

    auto email = (*json)["email"].asString();
    auto password = (*json)["password"].asString();

    AuthService::loginUser(
            email,
            password,
            [callback](const SessionDTO &session, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body;
                body["token"] = session.token;

                callback(jsonOK(body));
            }
    );
}

void AuthController::me(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    auto auth = req->getHeader("Authorization");
    if (auth.size() < 8 || auth.substr(0,7) != "Bearer ") {
        callback(jsonError(401, "Unauthorized"));
        return;
    }

    std::string token = auth.substr(7);

    AuthService::me(
            token,
            [callback](const UserDTO& user, const AppError& err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body;
                body["id"] = user.id;
                body["email"] = user.email;

                callback(jsonOK(body));
            }
    );
}

void AuthController::logout(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    auto auth = req->getHeader("Authorization");
    if (auth.size() < 8 || auth.substr(0,7) != "Bearer ") {
        callback(jsonError(401, "Unauthorized"));
        return;
    }

    std::string token = auth.substr(7);

    AuthService::logout(
            token,
            [callback](const AppError& err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value ok;
                ok["ok"] = true;

                callback(jsonOK(ok));
            }
    );
}