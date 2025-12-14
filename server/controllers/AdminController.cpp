#include "AdminController.h"

#include "core/RequestContextHelpers.h"
#include "mappers/RoleMapper.h"
#include "mappers/UserMapper.h"
#include "mappers/MapperRegistry.h"
#include "services/AdminService.h"

void AdminController::listUsers(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& cb
) {
    REQUIRE_ADMIN(req, cb);

    AdminService::listUsers(
            [cb](const std::vector<UserDTO>& users, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                for (const auto& u : users) {
                    arr.append(M.toJson(u));
                }

                cb(jsonOK(arr));
            }
    );
}

void AdminController::setRole(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& cb
) {
    REQUIRE_ADMIN(req, cb);       // admin-only
    REQUIRE_AUTH_USER(req, cb, actingUser);

    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_USER_ID) || !json->isMember(Const::JSON_ROLE)) {
        cb(jsonError(400, "userId and role are required"));
        return;
    }

    int targetUserId = (*json)[Const::JSON_USER_ID].asInt();
    std::string roleStr = (*json)[Const::JSON_ROLE].asString();

    auto role = fromString(roleStr);
    if (role == UserRole::Invalid) {
        cb(jsonError(400, "Invalid role. Allowed: user, moderator, admin"));
        return;
    }

    AdminService::setRole(
            targetUserId,
            role,
            actingUser.id,
            [cb](const UserDTO& user, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                cb(jsonOK(M.toJson(user)));
            }
    );
}