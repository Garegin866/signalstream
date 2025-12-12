#include "FeedRepository.h"

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
                    int itemId = row["item_id"].as<int>();

                    // Create item if not exists
                    if (!items.count(itemId)) {
                        FeedItemDTO dto;
                        dto.id = itemId;
                        dto.title = row["title"].as<std::string>();
                        dto.description = row["description"].as<std::string>();
                        dto.url = row["url"].as<std::string>();
                        items[itemId] = dto;
                    }

                    // Add tag
                    TagDTO tag;
                    tag.id = row["tag_id"].as<int>();
                    tag.name = row["tag_name"].as<std::string>();
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