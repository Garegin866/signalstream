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