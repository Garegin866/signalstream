#pragma once

#include "dto/NotificationDTO.h"
#include "repositories/NotificationRepository.h"
#include "repositories/UserRepository.h"
#include "core/AppError.h"

#include <drogon/drogon.h>

class NotificationService {
public:
    static void sendNotification(
            int userId,
            const std::string &type,
            const std::string &message,
            const std::string& entityType,
            int entityId,
            const std::function<void(const NotificationDTO&, const AppError&)>& cb
    );

    static void listNotifications(
            int userId,
            const std::function<void(const std::vector<NotificationDTO>&, const AppError&)>& cb
    );

    static void markAsRead(
            int notificationId,
            int requestingUserId,
            const std::function<void(const AppError&)>& cb
    );
};