#pragma once
#include <functional>
#include <optional>
#include "../dto/ItemDTO.h"
#include "../core/Error.h"

class ItemsService {
public:
    static void createItem(
            const std::string& title,
            const std::string& description,
            const std::string& url,
            const std::function<void(const ItemDTO&, const AppError&)>& cb
    );

    static void getItem(
            int itemId,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void updateItem(
            int itemId,
            const std::string& title,
            const std::string& description,
            const std::string& url,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void deleteItem(
            int itemId,
            const std::function<void(bool, const AppError&)>& cb
    );

    static void listItems(
            const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
    );
};