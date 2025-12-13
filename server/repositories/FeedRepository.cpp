#include "FeedRepository.h"

#include "core/Constants.h"

void FeedRepository::getFeedForUser(
        drogon::orm::DbClientPtr client,
        int userId,
        std::function<void(const std::vector<FeedItemDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT i.id AS item_id, i.title, i.description, i.url, "
            "       t.id AS tag_id, t.name AS tag_name "
            "FROM items i "
            "JOIN item_tags it ON i.id = it.item_id "
            "JOIN tags t ON it.tag_id = t.id "
            "WHERE it.tag_id IN (SELECT tag_id FROM user_tags WHERE user_id = $1) "
            "ORDER BY i.id DESC;",
            [cb](const drogon::orm::Result& r) {
                std::unordered_map<int, FeedItemDTO> items;

                for (const auto& row : r) {
                    int itemId = row[Const::COL_ITEM_ID].as<int>();

                    // Create item if not exists
                    if (!items.count(itemId)) {
                        FeedItemDTO dto;
                        dto.id = itemId;
                        dto.title = row[Const::COL_TITLE].as<std::string>();
                        dto.description = row[Const::COL_DESCRIPTION].as<std::string>();
                        dto.url = row[Const::COL_URL].as<std::string>();
                        items[itemId] = dto;
                    }

                    // Add tag
                    TagDTO tag;
                    tag.id = row[Const::COL_TAG_ID].as<int>();
                    tag.name = row[Const::COL_TAG_NAME].as<std::string>();
                    items[itemId].tags.push_back(tag);
                }

                // Convert map → vector
                std::vector<FeedItemDTO> result;
                result.reserve(items.size());
                for (auto& p : items)
                    result.push_back(std::move(p.second));

                cb(result, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            userId
    );
}