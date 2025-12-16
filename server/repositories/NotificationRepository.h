#pragma once

#include "dto/NotificationDTO.h"
#include "core/Error.h"

#include <drogon/drogon.h>

class NotificationRepository {
public:
    static void insert(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::string &type,
            const std::string &message,
            const std::function<void(const NotificationDTO&, const AppError&)>& cb
    );

    static void listByUser(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::function<void(const std::vector<NotificationDTO>&, const AppError&)>& cb
    );

    static void markRead(
            const drogon::orm::DbClientPtr& client,
            int notificationId,
            const std::function<void(const AppError&)>& cb
    );
};