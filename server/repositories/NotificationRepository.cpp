#include "NotificationRepository.h"

#include "core/Constants.h"
#include "mappers/MapperRegistry.h"
#include "mappers/NotificationMapper.h"

void NotificationRepository::insert(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::string &type,
        const std::string &message,
        const std::function<void(const NotificationDTO&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "INSERT INTO notifications (user_id, type, message) "
            "VALUES ($1, $2, $3) "
            "RETURNING id, user_id, type, message, created_at, read_at;",
            [cb](const drogon::orm::Result &r) {
                auto M = MapperRegistry<NotificationDTO, NotificationMapper>::get();
                cb(M.fromRow(r[0]), AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                cb({}, AppError::Database("Could not insert notification"));
            },
            userId, type, message
    );
}

void NotificationRepository::listByUser(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::function<void(const std::vector<NotificationDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, user_id, type, message, created_at, read_at "
            "FROM notifications WHERE user_id=$1 ORDER BY created_at DESC;",
            [cb](const drogon::orm::Result &r) {
                std::vector<NotificationDTO> list;
                list.reserve(r.size());

                auto M = MapperRegistry<NotificationDTO, NotificationMapper>::get();
                for (const auto &row : r) {
                    list.push_back(M.fromRow(row));
                }

                cb(list, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to fetch notifications"));
            },
            userId
    );
}

void NotificationRepository::markRead(
        const drogon::orm::DbClientPtr& client,
        int notificationId,
        const std::function<void(const AppError&)>& cb
) {
    client->execSqlAsync(
            "UPDATE notifications SET read_at=NOW() WHERE id=$1 RETURNING id;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(AppError::NotFound("Notification not found"));
                    return;
                }
                cb(AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(AppError::Database("Could not mark notification read"));
            },
            notificationId
    );
}