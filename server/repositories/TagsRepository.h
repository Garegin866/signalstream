#pragma once
#include <drogon/orm/DbClient.h>
#include "dto/TagDTO.h"
#include "core/Error.h"

class TagsRepository {
public:
    static void createTag(
            drogon::orm::DbClientPtr client,
            const std::string &name,
            std::function<void(const TagDTO&, const AppError&)> cb
    );

    static void listTags(
            drogon::orm::DbClientPtr client,
            std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
    );

    static void findById(
            drogon::orm::DbClientPtr client,
            int tagId,
            std::function<void(const std::optional<TagDTO>&, const AppError&)> cb
    );
};
