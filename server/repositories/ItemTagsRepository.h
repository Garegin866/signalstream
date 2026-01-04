#pragma once

#include <vector>
#include <functional>
#include <optional>
#include <drogon/orm/DbClient.h>

#include "dto/TagDTO.h"
#include "dto/ItemDTO.h"
#include "core/AppError.h"

class ItemTagsRepository {
public:
    static void attachTagsToItem(
            const drogon::orm::DbClientPtr& client,
            int itemId,
            const std::vector<int>& tagIds,
            const std::function<void(bool, const AppError&)>& cb
    );

    static void listTagsForItem(
            const drogon::orm::DbClientPtr& client,
            int itemId,
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );

    static void listItemsForTag(
            const drogon::orm::DbClientPtr& client,
            int tagId,
            const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
    );
};