#pragma once
#include <drogon/orm/DbClient.h>
#include "dto/TagDTO.h"
#include "core/Error.h"

class TagsRepository {
public:
    static void createTag(
            const drogon::orm::DbClientPtr& client,
            const std::string &name,
            const std::function<void(const TagDTO&, const AppError&)>& cb
    );

    static void listTags(
            const drogon::orm::DbClientPtr& client,
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );

    static void findById(
            const drogon::orm::DbClientPtr& client,
            int tagId,
            const std::function<void(const std::optional<TagDTO>&, const AppError&)>& cb
    );
};
