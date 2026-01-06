#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/ItemTagsRepository.h"

#include "dto/ItemDTO.h"
#include "dto/TagDTO.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

TEST_CASE("ItemTagsRepository::attachTagsToItem attaches tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto item = db::builder::createItem(client, "item1");
    auto tagA = db::builder::createTag(client, "tagA");
    auto tagB = db::builder::createTag(client, "tagB");

    std::promise<bool> ok;
    std::promise<AppError> err;

    ItemTagsRepository::attachTagsToItem(
            client,
            item.id,
            {tagA.id, tagB.id},
            [&](bool success, const AppError& e) {
                ok.set_value(success);
                err.set_value(e);
            }
    );

    REQUIRE(ok.get_future().get());
    REQUIRE_FALSE(err.get_future().get().hasError());
}

TEST_CASE("ItemTagsRepository::listTagsForItem returns attached tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto item = db::builder::createItem(client, "item1");
    auto tagA = db::builder::createTag(client, "alpha");
    auto tagB = db::builder::createTag(client, "beta");

    ItemTagsRepository::attachTagsToItem(client, item.id, {tagA.id, tagB.id},
                                         [](bool, const AppError&) {});

    std::promise<std::vector<TagDTO>> tags;
    std::promise<AppError> err;

    ItemTagsRepository::listTagsForItem(
            client,
            item.id,
            [&](const std::vector<TagDTO>& list, const AppError& e) {
                tags.set_value(list);
                err.set_value(e);
            }
    );

    auto res = tags.get_future().get();
    REQUIRE_FALSE(err.get_future().get().hasError());
    REQUIRE(res.size() == 2);
    REQUIRE(res[0].name == "alpha");
    REQUIRE(res[1].name == "beta");
}

TEST_CASE("ItemTagsRepository::listItemsForTag returns tagged items") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto item1 = db::builder::createItem(client, "item1");
    auto item2 = db::builder::createItem(client, "item2");
    auto tag = db::builder::createTag(client, "cpp");

    ItemTagsRepository::attachTagsToItem(client, item1.id, {tag.id},
                                         [](bool, const AppError&) {});
    ItemTagsRepository::attachTagsToItem(client, item2.id, {tag.id},
                                         [](bool, const AppError&) {});

    std::promise<std::vector<ItemDTO>> items;
    std::promise<AppError> err;

    ItemTagsRepository::listItemsForTag(
            client,
            tag.id,
            [&](const std::vector<ItemDTO>& list, const AppError& e) {
                items.set_value(list);
                err.set_value(e);
            }
    );

    auto res = items.get_future().get();
    REQUIRE_FALSE(err.get_future().get().hasError());
    REQUIRE(res.size() == 2);
}

TEST_CASE("ItemTagsRepository prevents duplicate attachment") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto item = db::builder::createItem(client, "item");
    auto tag  = db::builder::createTag(client, "dup");

    ItemTagsRepository::attachTagsToItem(client, item.id, {tag.id},
                                         [](bool, const AppError&) {});
    ItemTagsRepository::attachTagsToItem(client, item.id, {tag.id},
                                         [](bool, const AppError&) {});

    std::promise<std::vector<TagDTO>> tags;
    ItemTagsRepository::listTagsForItem(
            client,
            item.id,
            [&](const std::vector<TagDTO>& list, const AppError&) {
                tags.set_value(list);
            }
    );

    auto res = tags.get_future().get();
    REQUIRE(res.size() == 1); // no duplicates
}