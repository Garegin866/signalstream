#pragma once

#include <drogon/HttpController.h>

class NotificationsController : public drogon::HttpController<NotificationsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(NotificationsController::listNotifications, "/notifications", drogon::Get);
        ADD_METHOD_TO(NotificationsController::markRead, "/notifications/read", drogon::Post);
    METHOD_LIST_END

    static void listNotifications(
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr&)> &&callback
    );

    static void markRead(
            const drogon::HttpRequestPtr &req,
            std::function<void(const drogon::HttpResponsePtr&)> &&callback
    );
};
