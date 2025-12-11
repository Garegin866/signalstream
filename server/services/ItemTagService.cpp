#include "ItemTagService.h"
#include <drogon/drogon.h>
#include "../repositories/ItemTagsRepository.h"
#include "../repositories/ItemsRepository.h"
#include "../repositories/TagsRepository.h"

// Validate existence of item and tag before attaching
static void validateItemAndTag(
        drogon::orm::DbClientPtr client,
        int itemId,
        int tagId,
        std::function<void(bool, const AppError&)> cb,
        std::function<void()> onValid
) {
    // 1. Check item
    ItemsRepository::getItemById(
            client, itemId,
            [client, tagId, cb, onValid](const std::optional<ItemDTO>& item, const AppError& err) {
                if (err.hasError()) {
                    cb(false, err);
                    return;
                }
                if (!item) {
                    cb(false, AppError{ErrorType::NotFound, "Item not found"});
                    return;
                }

                // 2. Check tag
                TagsRepository::findById(
                        client,
                        tagId,
                        [cb, onValid](const std::optional<TagDTO>& tag, const AppError& err2) {
                            if (err2.hasError()) {
                                cb(false, err2);
                                return;
                            }
                            if (!tag) {
                                cb(false, AppError{ErrorType::NotFound, "Tag not found"});
                                return;
                            }

                            onValid();
                        }
                );
            }
    );
}

void ItemTagService::attachTag(
        int itemId,
        int tagId,
        std::function<void(bool, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    validateItemAndTag(
            client, itemId, tagId, cb,
            [client, itemId, tagId, cb]() {
                ItemTagsRepository::attachTagToItem(
                        client,
                        itemId,
                        tagId,
                        [cb](bool ok, const AppError& err) {
                            cb(ok, err);
                        }
                );
            }
    );
}

void ItemTagService::getTagsForItem(
        int itemId,
        std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    ItemTagsRepository::listTagsForItem(
            client,
            itemId,
            [cb](const std::vector<TagDTO>& tags, const AppError& err) {
                cb(tags, err);
            }
    );
}

void ItemTagService::removeTag(
        int itemId,
        int tagId,
        std::function<void(bool, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    ItemTagsRepository::removeTagFromItem(
            client,
            itemId,
            tagId,
            [cb](bool ok, const AppError& err) {
                cb(ok, err);
            }
    );
}