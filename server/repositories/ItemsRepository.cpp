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

void ItemsRepository::getItemsByTagIds(
        drogon::orm::DbClientPtr client,
        const std::vector<int>& tagIds,
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    auto query = std::string("SELECT DISTINCT i.id, i.title FROM items i WHERE i.id IN (");

    client->execSqlAsync(
            "SELECT DISTINCT i.id, i.title "
            "FROM items i "
            "JOIN item_tags it ON i.id = it.item_id "
            "WHERE it.tag_id = ANY($1) "
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
            toPgArray(tagIds)
    );
}