#include "ItemsService.h"

#include "repositories/ItemsRepository.h"
#include "repositories/UserTagsRepository.h"
#include "repositories/ItemTagsRepository.h"
#include "repositories/NotificationRepository.h"

#include <drogon/drogon.h>

void ItemsService::createItem(
        const std::string& title,
        const std::string& description,
        const std::string& url,
        const std::vector<int>& tagIds,
        const std::function<void(const ItemDTO&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::createItem(
            client, title, description, url,
            [client, tagIds, cb](const ItemDTO& created, const AppError& err) {
                if (err.hasError()) {
                    cb({}, err);
                    return;
                }

                if (tagIds.empty()) {
                    cb(created, {});
                    return;
                }

                ItemTagsRepository::attachTagsToItem(
                        client,
                        created.id,
                        tagIds,
                        [client, created, tagIds, cb](bool ok, const AppError& err2) {
                            if (err2.hasError()) {
                                cb({}, err2);
                                return;
                            }

                            UserTagsRepository::findUsersByTagIds(
                                    client,
                                    tagIds,
                                    [client, created, cb](const std::vector<int>& userIds, const AppError& err3) {
                                        if (err3.hasError()) {
                                            cb({}, err3);
                                            return;
                                        }

                                        NotificationRepository::insertBulkForUsers(
                                                client,
                                                userIds,
                                                "item_created",
                                                "A new item has been added: " + created.title,
                                                "item",
                                                created.id,
                                                [created, cb](const AppError& err4) {
                                                    if (err4.hasError()) {
                                                        LOG_ERROR << "Notification insert failed: " << err4.message;
                                                    }

                                                    cb(created, {});
                                                }
                                        );
                                    }
                            );
                        }
                );
            }
    );
}

void ItemsService::getItem(
        int itemId,
        const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::getItemById(
            client,
            itemId,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                cb(item, err);
            }
    );
}

void ItemsService::updateItem(
        int itemId,
        const std::optional<std::string>& title,
        const std::optional<std::string>& description,
        const std::optional<std::string>& url,
        const std::function<void(const std::optional<ItemDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::updateItem(
            client,
            itemId,
            title,
            description,
            url,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                cb(item, err);
            }
    );
}

void ItemsService::deleteItem(
        int itemId,
        const std::function<void(const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::deleteItem(
            client,
            itemId,
            [cb](const AppError& err) {
                cb(err);
            }
    );
}


void ItemsService::listItems(
        const Pagination& pagination,
        const std::function<void(const std::vector<ItemDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::listAll(
            client,
            pagination,
            [cb](const std::vector<ItemDTO>& items, const AppError& err) {
                cb(items, err);
            }
    );
}