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
                if (err.type != ErrorType::None) {
                    if (err.type == ErrorType::Duplicate)
                        callback(jsonError(400, err.message));
                    else if (err.type == ErrorType::Validation)
                        callback(jsonError(400, err.message));
                    else
                        callback(jsonError(500, err.message));
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
                if (err.type != ErrorType::None) {
                    if (err.type == ErrorType::Unauthorized)
                        callback(jsonError(401, err.message));
                    else if (err.type == ErrorType::Validation)
                        callback(jsonError(400, err.message));
                    else
                        callback(jsonError(500, err.message));
                    return;
                }

                Json::Value body;
                body["token"] = session.token;

                callback(jsonOK(body));
            }
    );
}