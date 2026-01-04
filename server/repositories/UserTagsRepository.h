#pragma once

#include <vector>
#include <optional>
#include <drogon/orm/DbClient.h>

#include "dto/TagDTO.h"
#include "core/AppError.h"

class UserTagsRepository {
public:
    static void attach(
            const drogon::orm::DbClientPtr& client,
            int userId,
            int tagId,
            const std::function<void(bool, const AppError&)>& cb
    );

    static void listForUser(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );

    static void findUsersByTagIds(
            const drogon::orm::DbClientPtr& client,
            const std::vector<int> &tagIds,
            const std::function<void(const std::vector<int>&, const AppError&)>& cb
    );
};