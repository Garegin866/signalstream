#pragma once
#include <vector>
#include <functional>
#include <optional>
#include <drogon/orm/DbClient.h>

#include "../dto/TagDTO.h"
#include "../dto/ItemDTO.h"
#include "../core/Error.h"

class ItemTagsRepository {
public:
    static void attachTagToItem(
            drogon::orm::DbClientPtr client,
            int itemId,
            int tagId,
            std::function<void(bool, const AppError&)> cb
    );

    static void listTagsForItem(
            drogon::orm::DbClientPtr client,
            int itemId,
            std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
    );

    static void listItemsForTag(
            drogon::orm::DbClientPtr client,
            int tagId,
            std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
    );

    static void removeTagFromItem(
            drogon::orm::DbClientPtr client,
            int itemId,
            int tagId,
            std::function<void(bool, const AppError&)> cb
    );
};