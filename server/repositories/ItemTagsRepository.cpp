#include "ItemTagsRepository.h"

#include "core/Constants.h"
#include "core/SqlUtils.h"
#include "mappers/MapperRegistry.h"
#include "mappers/ItemMapper.h"

void ItemTagsRepository::attachTagsToItem(
        const drogon::orm::DbClientPtr& client,
        int itemId,
        const std::vector<int>& tagIds,
        const std::function<void(bool, const AppError&)>& cb
) {
    if (tagIds.empty()) {
        cb(true, AppError{});
        return;
    }

    std::string sql = "INSERT INTO item_tags(item_id, tag_id) "
                      "SELECT $1, x "
                      "FROM unnest($2::bigint[]) AS x "
                      "ON CONFLICT DO NOTHING";

    const std::string arr = SqlUtils::toPgIntArrayLiteral(tagIds);

    client->execSqlAsync(
            sql,
            [cb](const drogon::orm::Result&) {
                cb(true, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(false, AppError::Database("Failed to attach tags to item"));
            },
            itemId,
            arr
    );
}

void ItemTagsRepository::listTagsForItem(
        const drogon::orm::DbClientPtr& client,
        int itemId,
        const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
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
                    dto.id = row[Const::COL_ID].as<int>();
                    dto.name = row[Const::COL_NAME].as<std::string>();
                    tags.push_back(dto);
                }

                cb(tags, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            itemId
    );
}

void ItemTagsRepository::listItemsForTag(
        const drogon::orm::DbClientPtr& client,
        int tagId,
        const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
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

                auto& M = MapperRegistry<ItemDTO, ItemMapper>::get();
                for (const auto& row : r) {
                    items.push_back(M.fromRow(row));
                }

                cb(items, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Database error"));
            },
            tagId
    );
}