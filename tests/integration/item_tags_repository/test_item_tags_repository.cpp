#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/ItemTagsRepository.h"
#include "repositories/ItemsRepository.h"
#include "repositories/TagsRepository.h"

#include "dto/ItemDTO.h"
#include "dto/TagDTO.h"
#include "core/AppError.h"

using drogon::orm::DbClientPtr;

namespace {

    DbClientPtr makeClient() {
        static DbClientPtr client;

        if (!client) {
            client = drogon::orm::DbClient::newPgClient(
                    "host=127.0.0.1 port=5432 dbname=signaldb_test user=signaluser password=signalpass",
                    1
            );
        }

        return client;
    }

    void resetDb(const DbClientPtr& client) {
        client->execSqlSync("DELETE FROM item_tags;");
        client->execSqlSync("DELETE FROM items;");
        client->execSqlSync("DELETE FROM tags;");
    }

    ItemDTO createItem(const DbClientPtr& client, const std::string& title) {
        std::promise<ItemDTO> p;
        std::promise<AppError> e;

        ItemsRepository::createItem(
                client,
                title,
                "desc",
                "url",
                [&](const ItemDTO& item, const AppError& err) {
                    p.set_value(item);
                    e.set_value(err);
                }
        );

        auto err = e.get_future().get();
        REQUIRE_FALSE(err.hasError());

        return p.get_future().get();
    }

    TagDTO createTag(const DbClientPtr& client, const std::string& name) {
        std::promise<TagDTO> p;
        std::promise<AppError> e;

        TagsRepository::createTag(
                client,
                name,
                [&](const TagDTO& tag, const AppError& err) {
                    p.set_value(tag);
                    e.set_value(err);
                }
        );

        auto err = e.get_future().get();
        REQUIRE_FALSE(err.hasError());

        return p.get_future().get();
    }

} // namespace

// ------------------------------------------------------------------
// Tests
// ------------------------------------------------------------------

TEST_CASE("ItemTagsRepository::attachTagsToItem attaches tags") {
    auto client = makeClient();
    resetDb(client);

    auto item = createItem(client, "item1");
    auto tagA = createTag(client, "tagA");
    auto tagB = createTag(client, "tagB");

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
    auto client = makeClient();
    resetDb(client);

    auto item = createItem(client, "item1");
    auto tagA = createTag(client, "alpha");
    auto tagB = createTag(client, "beta");

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
    auto client = makeClient();
    resetDb(client);

    auto item1 = createItem(client, "item1");
    auto item2 = createItem(client, "item2");
    auto tag = createTag(client, "cpp");

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
    auto client = makeClient();
    resetDb(client);

    auto item = createItem(client, "item");
    auto tag  = createTag(client, "dup");

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