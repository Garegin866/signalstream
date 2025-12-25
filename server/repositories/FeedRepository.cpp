#include "FeedRepository.h"

#include "core/Constants.h"
#include "core/SqlUtils.h"

void FeedRepository::getFeedForUser(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const Pagination& pagination,
        const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT i.id, i.title, i.description, i.url, i.created_at "
            "FROM items i "
            "WHERE EXISTS ( "
            "   SELECT 1 FROM item_tags it "
            "   JOIN user_tags ut ON ut.tag_id = it.tag_id "
            "   WHERE it.item_id = i.id AND ut.user_id = $1 "
            ") "
            "ORDER BY i.created_at DESC "
            "LIMIT $2::int OFFSET $3::int;",
            [client, cb](const drogon::orm::Result& r) {
                if (r.empty()) {
                    cb({}, AppError{});
                    return;
                }

                std::vector<FeedItemDTO> items;
                items.reserve(r.size());
                std::vector<int> itemIds;
                itemIds.reserve(r.size());

                for (const auto& row : r) {
                    FeedItemDTO dto;
                    dto.id = row["id"].as<int>();
                    dto.title = row["title"].as<std::string>();
                    dto.description = row["description"].as<std::string>();
                    dto.url = row["url"].as<std::string>();
                    items.push_back(dto);
                    itemIds.push_back(dto.id);
                }

                const std::string pgArr = SqlUtils::toPgIntArrayLiteral(itemIds);

                client->execSqlAsync(
                        "SELECT it.item_id, t.id, t.name "
                        "FROM item_tags it "
                        "JOIN tags t ON t.id = it.tag_id "
                        "WHERE it.item_id = ANY($1::int[]);",
                        [items = std::move(items), cb](const drogon::orm::Result& tr) mutable {
                            std::unordered_map<int, size_t> index;
                            for (size_t i = 0; i < items.size(); ++i)
                                index[items[i].id] = i;

                            for (const auto& row : tr) {
                                int itemId = row["item_id"].as<int>();
                                TagDTO tag;
                                tag.id = row["id"].as<int>();
                                tag.name = row["name"].as<std::string>();
                                items[index[itemId]].tags.push_back(tag);
                            }

                            cb(items, AppError{});
                        },
                        [cb](const std::exception_ptr&) {
                            cb({}, AppError::Database("Failed to load tags"));
                        },
                        pgArr
                );
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to load feed"));
            },
            userId,
            pagination.limit,
            pagination.offset
    );
}