#pragma once

#include "dto/TagDTO.h"
#include "core/AppError.h"

#include <functional>
#include <drogon/orm/DbClient.h>

class TagsService {
public:
    explicit TagsService(drogon::orm::DbClientPtr client);

    void createTag(
            const std::string &name,
            const std::function<void(const TagDTO&, const AppError&)>& cb
    );

    void listTags(
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );

private:
    drogon::orm::DbClientPtr client_;
};
