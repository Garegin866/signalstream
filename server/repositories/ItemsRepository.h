#pragma once
#include <optional>
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

    static void createItem(
            drogon::orm::DbClientPtr client,
            const std::string& title,
            const std::string& description,
            const std::string& url,
            std::function<void(const ItemDTO&, const AppError&)> cb
    );

    static void getItemById(
            drogon::orm::DbClientPtr client,
            int itemId,
            std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
    );

    static void updateItem(
            drogon::orm::DbClientPtr client,
            int itemId,
            const std::string& title,
            const std::string& description,
            const std::string& url,
            std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
    );

    static void deleteItem(
            drogon::orm::DbClientPtr client,
            int itemId,
            std::function<void(bool, const AppError&)> cb
    );

    static void listAll(
            drogon::orm::DbClientPtr client,
            std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
    );
};