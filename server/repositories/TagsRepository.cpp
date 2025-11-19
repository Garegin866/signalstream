#include "TagsRepository.h"

#include <drogon/orm/DbClient.h>

void TagsRepository::createTag(
        drogon::orm::DbClientPtr client,
        const std::string &name,
        std::function<void(const TagDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO tags (name) VALUES ($1) RETURNING id, name;",
            [cb](const drogon::orm::Result &r) {
                TagDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.name = r[0]["name"].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &e) {
                    std::string msg = e.base().what();
                    if (msg.find("duplicate key") != std::string::npos) {
                        cb({}, AppError{ErrorType::Duplicate, "Tag already exists"});
                        return;
                    }
                    cb({}, AppError{ErrorType::Database, "Database error"});
                } catch (...) {
                    cb({}, AppError{ErrorType::Unknown, "Internal error"});
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
                    dto.id = row["id"].as<int>();
                    dto.name = row["name"].as<std::string>();
                    list.push_back(std::move(dto));
                }

                cb(list, AppError{});
            },

            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &) {
                    cb({}, AppError{ErrorType::Database, "Database error"});
                } catch (...) {
                    cb({}, AppError{ErrorType::Unknown, "Internal error"});
                }
            }
    );
}
