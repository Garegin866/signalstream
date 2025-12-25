#include "UserTagsRepository.h"

#include <drogon/orm/Exception.h>

#include "core/Constants.h"
#include "core/SqlUtils.h"

void UserTagsRepository::attach(
        const drogon::orm::DbClientPtr& client,
        int userId,
        int tagId,
        const std::function<void(bool, const AppError&)>& cb
) {
    client->execSqlAsync(
            "INSERT INTO user_tags (user_id, tag_id) VALUES ($1, $2);",
            [cb](const drogon::orm::Result&) {
                cb(true, AppError{}); // success
            },
            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &e) {
                    std::string msg = e.base().what();

                    // Duplicate key = user already has this tag
                    if (msg.find("duplicate key value violates unique constraint") != std::string::npos) {
                        cb(false, AppError::Duplicate("User already has this tag"));
                        return;
                    }

                    cb(false, AppError::Database("Database error"));
                } catch (...) {
                    cb(false, AppError::NotFound("Unknown error"));
                }
            },
            userId, tagId
    );
}

void UserTagsRepository::listForUser(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT t.id, t.name "
            "FROM tags t "
            "JOIN user_tags ut ON ut.tag_id = t.id "
            "WHERE ut.user_id = $1 "
            "ORDER BY t.name;",
            [cb](const drogon::orm::Result &r) {
                std::vector<TagDTO> tags;
                tags.reserve(r.size());

                for (const auto &row : r) {
                    TagDTO dto;
                    dto.id = row[Const::COL_ID].as<int>();
                    dto.name = row[Const::COL_NAME].as<std::string>();
                    tags.push_back(dto);
                }

                cb(tags, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            userId
    );
}

void UserTagsRepository::findUsersByTagIds(
        const drogon::orm::DbClientPtr& client,
        const std::vector<int> &tagIds,
        const std::function<void(const std::vector<int>&, const AppError&)>& cb
) {
    if (tagIds.empty()) {
        cb({}, AppError{});
        return;
    }

    const std::string pgArr = SqlUtils::toPgIntArrayLiteral(tagIds);

    client->execSqlAsync(
            "SELECT DISTINCT user_id "
            "FROM user_tags "
            "WHERE tag_id = ANY($1::int[]);",
            [cb](const drogon::orm::Result& res) {
                std::vector<int> userIds;
                userIds.reserve(res.size());

                for (const auto& row : res) {
                    userIds.push_back(row[Const::COL_USER_ID].as<int>());
                }
                cb(userIds, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            pgArr
    );
}