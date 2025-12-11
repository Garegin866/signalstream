#include "ItemTagsRepository.h"

// -----------------------------------------------------
// 1. ATTACH TAG TO ITEM
// -----------------------------------------------------
void ItemTagsRepository::attachTagToItem(
        drogon::orm::DbClientPtr client,
        int itemId,
        int tagId,
        std::function<void(bool, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO item_tags (item_id, tag_id) VALUES ($1, $2);",
            [cb](const drogon::orm::Result&) {
                cb(true, AppError{});
            },
            [cb](const std::exception_ptr& eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const std::exception &e) {
                    std::string msg = e.what();
                    if (msg.find("duplicate key") != std::string::npos) {
                        cb(false, AppError{ErrorType::Duplicate, "Tag already attached"});
                        return;
                    }
                    cb(false, AppError{ErrorType::Database, "Database error"});
                }
            },
            itemId, tagId
    );
}


// -----------------------------------------------------
// 2. LIST TAGS FOR ITEM
// -----------------------------------------------------
void ItemTagsRepository::listTagsForItem(
        drogon::orm::DbClientPtr client,
        int itemId,
        std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT t.id, t.name "
            "FROM tags t "
            "JOIN item_tags it ON t.id = it.tag_id "
            "WHERE it.item_id = $1 "
            "ORDER BY t.name;",
            [cb](const drogon::orm::Result& r) {
                std::vector<TagDTO> tags;
                tags.reserve(r.size());

                for (const auto& row : r) {
                    TagDTO dto;
                    dto.id = row["id"].as<int>();
                    dto.name = row["name"].as<std::string>();
                    tags.push_back(dto);
                }

                cb(tags, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError{ErrorType::Database, "Database error"});
            },
            itemId
    );
}


// -----------------------------------------------------
// 3. LIST ITEMS FOR A TAG (Optional helper)
// -----------------------------------------------------
void ItemTagsRepository::listItemsForTag(
        drogon::orm::DbClientPtr client,
        int tagId,
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT i.id, i.title "
            "FROM items i "
            "JOIN item_tags it ON i.id = it.item_id "
            "WHERE it.tag_id = $1 "
            "ORDER BY i.created_at DESC;",
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
            tagId
    );
}


// -----------------------------------------------------
// 4. REMOVE TAG FROM ITEM
// -----------------------------------------------------
void ItemTagsRepository::removeTagFromItem(
        drogon::orm::DbClientPtr client,
        int itemId,
        int tagId,
        std::function<void(bool, const AppError&)> cb
) {
    client->execSqlAsync(
            "DELETE FROM item_tags WHERE item_id = $1 AND tag_id = $2;",
            [cb](const drogon::orm::Result&) {
                cb(true, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(false, AppError{ErrorType::Database, "Database error"});
            },
            itemId, tagId
    );
}