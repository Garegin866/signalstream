#include "AuthController.h"

#include "services/AuthService.h"
#include "core/Response.h"
#include "mappers/RoleMapper.h"
#include "mappers/UserMapper.h"
#include "mappers/MapperRegistry.h"
#include "core/RequestContextHelpers.h"

using namespace drogon;

void AuthController::registerUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    REQUIRE_ADMIN(req, callback);

    auto json = req->getJsonObject();
    if (!json) {
        callback(jsonError(400, "Invalid body"));
        return;
    }

    auto email = (*json)[Const::JSON_EMAIL].asString();
    auto password = (*json)[Const::JSON_PASSWORD].asString();

    AuthService::registerUser(
            email,
            password,
            [callback](const UserDTO &user, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                callback(jsonCreated(M.toJson(user)));
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

    auto email = (*json)[Const::JSON_EMAIL].asString();
    auto password = (*json)[Const::JSON_PASSWORD].asString();

    AuthService::loginUser(
            email,
            password,
            [callback](const SessionDTO &session, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body;
                body[Const::JSON_TOKEN] = session.token;

                callback(jsonOK(body));
            }
    );
}

void AuthController::me(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    auto& M = MapperRegistry<UserDTO, UserMapper>::get();
    Json::Value body = M.toJson(user);

    callback(jsonOK(body));
}

void AuthController::logout(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    REQUIRE_AUTH_USER_WITH_TOKEN(req, callback, user, token)

    AuthService::logout(
            token,
            [callback](const AppError& err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value ok;
                ok[Const::JSON_OK] = true;

                callback(jsonOK(ok));
            }
    );
}