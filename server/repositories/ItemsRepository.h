#pragma once
#include <vector>
#include <drogon/orm/DbClient.h>
#include "../dto/ItemDTO.h"
#include "../core/Error.h"

class ItemsRepository {
public:
    static void getItemsByTagIds(
            drogon::orm::DbClientPtr client,
            const std::vector<int>& tagIds,
            std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
    );
};