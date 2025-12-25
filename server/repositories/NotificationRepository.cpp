#include "NotificationRepository.h"

#include "core/Constants.h"
#include "core/SqlUtils.h"
#include "mappers/MapperRegistry.h"
#include "mappers/NotificationMapper.h"

void NotificationRepository::insert(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::string &type,
        const std::string &message,
        const std::string& entityType,
        int entityId,
        const std::function<void(const NotificationDTO&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "INSERT INTO notifications "
            "(user_id, type, message, entity_type, entity_id) "
            "VALUES ($1, $2, $3, $4, $5) "
            "ON CONFLICT (user_id, entity_type, entity_id) DO NOTHING "
            "RETURNING id, user_id, type, message, entity_type, entity_id, created_at, read_at;",
            [cb](const drogon::orm::Result& r) {
                // If empty → conflict → already existed
                auto M = MapperRegistry<NotificationDTO, NotificationMapper>::get();
                cb(M.fromRow(r[0]), AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to insert notification"));
            },
            userId, type, message, entityType, entityId
    );
}

void NotificationRepository::listByUser(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::function<void(const std::vector<NotificationDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT * "
            "FROM notifications"
            "WHERE user_id=$1 "
            "ORDER BY created_at DESC;",
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
            "UPDATE notifications "
            "SET read_at=NOW() "
            "WHERE id=$1 AND read_at IS NULL "
            "RETURNING id;",
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

void NotificationRepository::insertBulkForUsers(
        const drogon::orm::DbClientPtr& client,
        const std::vector<int>& userIds,
        const std::string& type,
        const std::string& message,
        const std::string& entityType,
        int entityId,
        const std::function<void(const AppError&)>& cb
) {
    if (userIds.empty()) {
        cb(AppError{});
        return;
    }

    const std::string arr = SqlUtils::toPgIntArrayLiteral(userIds);

    client->execSqlAsync(
            "INSERT INTO notifications (user_id, type, message, entity_type, entity_id) "
            "SELECT u, $2, $3, $4, $5 "
            "FROM unnest($1::int[]) AS u "
            "ON CONFLICT (user_id, entity_type, entity_id) DO NOTHING;",
            [cb](const drogon::orm::Result&) {
                cb(AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(AppError::Database("Failed to insert notifications"));
            },
            arr, type, message, entityType, entityId
    );
}