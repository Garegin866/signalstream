#include "TagsRepository.h"

#include <drogon/orm/DbClient.h>

#include "../core/Constants.h"

void TagsRepository::createTag(
        drogon::orm::DbClientPtr client,
        const std::string &name,
        std::function<void(const TagDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO tags (name) VALUES ($1) RETURNING id, name;",
            [cb](const drogon::orm::Result &r) {
                TagDTO dto;
                dto.id = r[0][Const::COL_ID].as<int>();
                dto.name = r[0][Const::COL_NAME].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &e) {
                    std::string msg = e.base().what();
                    if (msg.find("duplicate key") != std::string::npos) {
                        cb({}, AppError::Duplicate("Tag already exists"));
                        return;
                    }
                    cb({}, AppError::Database("Database error"));
                } catch (...) {
                    cb({}, AppError::NotFound("Internal error"));
                }
            },
            name
    );
}

void TagsRepository::listTags(
        drogon::orm::DbClientPtr client,
        std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, name FROM tags ORDER BY name;",
            [cb](const drogon::orm::Result &r) {
                std::vector<TagDTO> list;
                list.reserve(r.size());

                for (const auto &row : r) {
                    TagDTO dto;
                    dto.id = row[Const::COL_ID].as<int>();
                    dto.name = row[Const::COL_NAME].as<std::string>();
                    list.push_back(std::move(dto));
                }

                cb(list, AppError{});
            },

            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &) {
                    cb({}, AppError::Database("Database error"));
                } catch (...) {
                    cb({}, AppError::NotFound("Internal error"));
                }
            }
    );
}

void TagsRepository::findById(
        drogon::orm::DbClientPtr client,
        int tagId,
        std::function<void(const std::optional<TagDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, name FROM tags WHERE id=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb({}, AppError::NotFound("Tag not found"));
                    return;
                }

                TagDTO dto;
                dto.id = r[0][Const::COL_ID].as<int>();
                dto.name = r[0][Const::COL_NAME].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            tagId
    );
}
