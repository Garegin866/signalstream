#pragma once

#include "dto/ItemDTO.h"
#include "core/AppError.h"
#include "pagination/Pagination.h"

#include <optional>
#include <vector>

#include <drogon/orm/DbClient.h>

class ItemsRepository {
public:
    static void getItemsByTagIds(
            const drogon::orm::DbClientPtr& client,
            const std::vector<int>& tagIds,
            const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
    );

    static void createItem(
            const drogon::orm::DbClientPtr& client,
            const std::string& title,
            const std::string& description,
            const std::string& url,
            const std::function<void(const ItemDTO&, const AppError&)>& cb
    );

    static void getItemById(
            const drogon::orm::DbClientPtr& client,
            int itemId,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void updateItem(
            const drogon::orm::DbClientPtr& client,
            int itemId,
            const std::optional<std::string>& title,
            const std::optional<std::string>& description,
            const std::optional<std::string>& url,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void deleteItem(
            const drogon::orm::DbClientPtr& client,
            int itemId,
            const std::function<void(const AppError&)>& cb
    );

    static void listAll(
            const drogon::orm::DbClientPtr& client,
            const Pagination& pagination,
            const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
    );
};