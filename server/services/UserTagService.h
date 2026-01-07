#pragma once

#include "core/AppError.h"

#include <functional>
#include <drogon/orm/DbClient.h>

struct TagDTO;

class UserTagService {
public:
    explicit UserTagService(drogon::orm::DbClientPtr client);

    void attachTag(
            int userId,
            int tagId,
            const std::function<void(const AppError&)>& cb
    );

    void listUserTags(
            int userId,
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );

private:
    drogon::orm::DbClientPtr client_;
};