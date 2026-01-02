#include "AdminController.h"

#include "core/RequestContextHelpers.h"
#include "mappers/RoleMapper.h"
#include "mappers/UserMapper.h"
#include "mappers/MapperRegistry.h"
#include "services/AdminService.h"
#include "dto/HealthDTO.h"

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
    REQUIRE_ADMIN(req, cb);
    REQUIRE_AUTH_USER(req, cb, actingUser)

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
            actingUser.id,
            targetUserId,
            role,
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

void AdminController::listModerators(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& callback
) {
    REQUIRE_ADMIN(req, callback);

    AdminService::listModerators(
            [callback](const std::vector<UserDTO>& users, const AppError& err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value arr(Json::arrayValue);

                auto M = MapperRegistry<UserDTO, UserMapper>::get();
                for (const auto& u : users) {
                    arr.append(M.toJson(u));
                }

                Json::Value body;
                body[Const::JSON_MODERS] = arr;

                callback(jsonOK(body));
            }
    );
}

void AdminController::health(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& cb
) {
    AdminService::health(
            [cb](const HealthDTO& dto) {
                Json::Value body;
                body["status"] = dto.status;
                body["db"] = dto.db;
                body["uptime_sec"] = dto.uptimeSec;
                body["timestamp"] = dto.timestamp;
                body["version"] = dto.version;

                cb(jsonOK(body));
            }
    );
}