#include "NotificationController.h"

#include "core/RequestContextHelpers.h"
#include "core/Response.h"
#include "core/Constants.h"
#include "services/NotificationService.h"
#include "dto/NotificationDTO.h"
#include "mappers/MapperRegistry.h"
#include "mappers/NotificationMapper.h"

void NotificationsController::listNotifications(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    NotificationService::listNotifications(
            user.id,
            [callback, req](const std::vector<NotificationDTO> &list, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err, req));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                auto& M = MapperRegistry<NotificationDTO, NotificationMapper>::get();
                for (const auto &n : list) {
                    arr.append(M.toJson(n));
                }

                Json::Value body;
                body[Const::JSON_NOTIFICATIONS] = arr;
                callback(jsonOK(body));
            }
    );
}


void NotificationsController::markRead(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    auto json = req->getJsonObject();
    if (!json || !(*json).isMember(Const::JSON_ID)) {
        callback(makeErrorResponse(AppError::Validation("Missing notification id"), req));
        return;
    }

    int notificationId = (*json)[Const::JSON_ID].asInt();

    NotificationService::markAsRead(
            notificationId,
            user.id,
            [callback, req](const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err, req));
                    return;
                }

                Json::Value ok;
                ok[Const::JSON_READ] = true;
                callback(jsonOK(ok));
            }
    );
}
