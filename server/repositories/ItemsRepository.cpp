#include "ItemsRepository.h"

static std::string toPgArray(const std::vector<int>& ids) {
    std::string out = "{";
    for (size_t i = 0; i < ids.size(); ++i) {
        out += std::to_string(ids[i]);
        if (i + 1 < ids.size()) out += ",";
    }
    out += "}";
    return out;
}

void ItemsRepository::getItemById(
        drogon::orm::DbClientPtr client,
        int itemId,
        std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, title, description, url FROM items WHERE id=$1;",
            [cb](const drogon::orm::Result& r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError{});
                    return;
                }

                ItemDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.title = r[0]["title"].as<std::string>();
                dto.description = r[0]["description"].as<std::string>();
                dto.url = r[0]["url"].as<std::string>();

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError{ErrorType::Database, "Database error"});
            },
            itemId
    );
}

void ItemsRepository::updateItem(
        drogon::orm::DbClientPtr client,
        int itemId,
        const std::string& title,
        const std::string& description,
        const std::string& url,
        std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "UPDATE items "
            "SET title=$2, description=$3, url=$4 "
            "WHERE id=$1 "
            "RETURNING id, title, description, url;",
            [cb](const drogon::orm::Result& r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError{ErrorType::NotFound, "Item not found"});
                    return;
                }

                ItemDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.title = r[0]["title"].as<std::string>();
                dto.description = r[0]["description"].as<std::string>();
                dto.url = r[0]["url"].as<std::string>();

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError{ErrorType::Database, "Database error"});
            },
            itemId, title, description, url
    );
}

void ItemsRepository::deleteItem(
        drogon::orm::DbClientPtr client,
        int itemId,
        std::function<void(bool, const AppError&)> cb
) {
    client->execSqlAsync(
            "DELETE FROM items WHERE id=$1;",
            [cb](const drogon::orm::Result&) {
                cb(true, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(false, AppError{ErrorType::Database, "Failed to delete item"});
            },
            itemId
    );
}

void ItemsRepository::listAll(
        drogon::orm::DbClientPtr client,
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, title, description, url FROM items ORDER BY id DESC;",
            [cb](const drogon::orm::Result& r) {
                std::vector<ItemDTO> items;
                items.reserve(r.size());

                for (const auto& row : r) {
                    ItemDTO dto;
                    dto.id = row["id"].as<int>();
                    dto.title = row["title"].as<std::string>();
                    dto.description = row["description"].as<std::string>();
                    dto.url = row["url"].as<std::string>();
                    items.push_back(dto);
                }

                cb(items, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError{ErrorType::Database, "Database error"});
            }
    );
}

void ItemsRepository::createItem(
        drogon::orm::DbClientPtr client,
        const std::string& title,
        const std::string& description,
        const std::string& url,
        std::function<void(const ItemDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO items (title, description, url) "
            "VALUES ($1, $2, $3) RETURNING id, title, description, url;",
            [cb](const drogon::orm::Result& r) {
                ItemDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.title = r[0]["title"].as<std::string>();
                dto.description = r[0]["description"].as<std::string>();
                dto.url = r[0]["url"].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr& eptr) {
                cb({}, AppError{ErrorType::Database, "Failed to create item"});
            },
            title, description, url
    );
}

void ItemsRepository::getItemsByTagIds(
        drogon::orm::DbClientPtr client,
        const std::vector<int>& tagIds,
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    std::string arr = toPgArray(tagIds);

    client->execSqlAsync(
            "SELECT DISTINCT i.id, i.title "
            "FROM items i "
            "JOIN item_tags it ON i.id = it.item_id "
            "WHERE it.tag_id = ANY($1::int[]) "
            "ORDER BY i.id DESC;",
            [cb](const drogon::orm::Result& r) {
                std::vector<ItemDTO> items;
                items.reserve(r.size());

                for (const auto& row : r) {
                    ItemDTO dto;
                    dto.id = row["id"].as<int>();
                    dto.title = row["title"].as<std::string>();
                    items.push_back(dto);
                }

                cb(items, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError{ErrorType::Database, "Database error"});
            },
            arr
    );
}