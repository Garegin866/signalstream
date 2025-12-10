#pragma once
#include <vector>
#include <optional>
#include <drogon/orm/DbClient.h>
#include "../dto/TagDTO.h"
#include "../core/Error.h"

class UserTagsRepository {
public:
    static void attach(
            drogon::orm::DbClientPtr client,
            int userId,
            int tagId,
            std::function<void(bool, const AppError&)> cb
    );

    static void listForUser(
            drogon::orm::DbClientPtr client,
            int userId,
            std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
    );
};