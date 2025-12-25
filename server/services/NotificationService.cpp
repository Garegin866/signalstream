#include "NotificationService.h"

void NotificationService::sendNotification(
        int userId,
        const std::string &type,
        const std::string &message,
        const std::string& entityType,
        int entityId,
        const std::function<void(const NotificationDTO&, const AppError&)>& cb
) {
    auto db = drogon::app().getDbClient();

    // V1: no validation on "type" – free-form text is fine
    if (type.empty()) {
        cb({}, AppError::Validation("Notification type cannot be empty"));
        return;
    }
    if (message.empty()) {
        cb({}, AppError::Validation("Notification message cannot be empty"));
        return;
    }

    NotificationRepository::insert(db, userId, type, message, entityType, entityId, cb);
}


void NotificationService::listNotifications(
        int userId,
        const std::function<void(const std::vector<NotificationDTO>&, const AppError&)>& cb
) {
    auto db = drogon::app().getDbClient();

    if (userId <= 0) {
        cb({}, AppError::Validation("Invalid user id"));
        return;
    }

    NotificationRepository::listByUser(db, userId, cb);
}

void NotificationService::markAsRead(
        int notificationId,
        int requestingUserId,
        const std::function<void(const AppError&)>& cb
) {
    auto db = drogon::app().getDbClient();

    if (notificationId <= 0) {
        cb(AppError::Validation("Invalid notification id"));
        return;
    }

    // First load the notification to check user ownership
    NotificationRepository::listByUser(
            db,
            requestingUserId,
            [notificationId, requestingUserId, cb, db](const std::vector<NotificationDTO> &list, const AppError &err) {
                (void)requestingUserId; // unused

                if (err.hasError()) {
                    cb(err);
                    return;
                }

                bool found = false;
                for (const auto &n : list) {
                    if (n.id == notificationId) {
                        found = true;
                        break;
                    }
                }

                if (!found) {
                    cb(AppError::Unauthorized("Cannot modify other user's notifications"));
                    return;
                }

                // Now perform update
                NotificationRepository::markRead(db, notificationId, cb);
            }
    );
}