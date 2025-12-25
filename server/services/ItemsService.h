#pragma once

#include "core/Error.h"
#include "dto/ItemDTO.h"
#include "pagination/Pagination.h"

#include <functional>
#include <optional>

class ItemsService {
public:
    static void createItem(
            const std::string& title,
            const std::string& description,
            const std::string& url,
            const std::vector<int>& tagIds,
            const std::function<void(const ItemDTO&, const AppError&)>& cb
    );

    static void getItem(
            int itemId,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void updateItem(
            int itemId,
            const std::optional<std::string>& title,
            const std::optional<std::string>& description,
            const std::optional<std::string>& url,
            const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
    );

    static void deleteItem(
            int itemId,
            const std::function<void(const AppError&)>& cb
    );

    static void listItems(
            const Pagination& pagination,
            const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
    );
};