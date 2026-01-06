#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/ItemsRepository.h"
#include "dto/ItemDTO.h"
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
        client->execSqlSync("TRUNCATE TABLE item_tags, tags, items, user_tags RESTART IDENTITY;");
    }

    ItemDTO createItem(
            const DbClientPtr& client,
            const std::string& title = "Title",
            const std::string& desc  = "Desc",
            const std::string& url   = "https://example.com"
    ) {
        std::promise<ItemDTO> itemPromise;
        std::promise<AppError> errorPromise;

        ItemsRepository::createItem(
                client,
                title,
                desc,
                url,
                [&](const ItemDTO& item, const AppError& e) {
                    itemPromise.set_value(item);
                    errorPromise.set_value(e);
                }
        );

        auto result = itemPromise.get_future().get();
        auto err    = errorPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
        REQUIRE(result.id >= 0);

        return result;
    }

    int createTag(const DbClientPtr& client, const std::string& name) {
        auto r = client->execSqlSync(
                "INSERT INTO tags (name) VALUES ($1) RETURNING id;",
                name
        );
        return r[0][0].as<int>();
    }

    void linkItemTag(
            const DbClientPtr& client,
            int itemId,
            int tagId
    ) {
        client->execSqlSync(
                "INSERT INTO item_tags (item_id, tag_id) VALUES ($1, $2);",
                itemId,
                tagId
        );
    }

} // namespace

// ------------------------------------------------------------
// Tests
// ------------------------------------------------------------

TEST_CASE("ItemsRepository::createItem inserts item") {
    auto client = makeClient();
    resetDb(client);

    auto item = createItem(client, "A", "B", "C");

    REQUIRE(item.title == "A");
    REQUIRE(item.description == "B");
    REQUIRE(item.url == "C");
}

TEST_CASE("ItemsRepository::getItemById returns item") {
    auto client = makeClient();
    resetDb(client);

    auto created = createItem(client);

    std::promise<std::optional<ItemDTO>> itemPromise;
    std::promise<AppError> errorPromise;

    ItemsRepository::getItemById(
            client,
            created.id,
            [&](const std::optional<ItemDTO>& i, const AppError& e) {
                itemPromise.set_value(i);
                errorPromise.set_value(e);
            }
    );

    auto found = itemPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(found.has_value());
    REQUIRE(found->id == created.id);
}

TEST_CASE("ItemsRepository::getItemById returns nullopt for unknown id") {
    auto client = makeClient();
    resetDb(client);

    std::promise<std::optional<ItemDTO>> itemPromise;
    std::promise<AppError> errorPromise;

    ItemsRepository::getItemById(
            client,
            99999,
            [&](const std::optional<ItemDTO>& i, const AppError& e) {
                itemPromise.set_value(i);
                errorPromise.set_value(e);
            }
    );

    auto found = itemPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE_FALSE(found.has_value());
}

TEST_CASE("ItemsRepository::updateItem updates fields") {
    auto client = makeClient();
    resetDb(client);

    auto created = createItem(client);

    std::promise<std::optional<ItemDTO>> itemPromise;
    std::promise<AppError> errorPromise;

    ItemsRepository::updateItem(
            client,
            created.id,
            std::string("New title"),
            std::nullopt,
            std::string("https://new.url"),
            [&](const std::optional<ItemDTO>& i, const AppError& e) {
                itemPromise.set_value(i);
                errorPromise.set_value(e);
            }
    );

    auto updated = itemPromise.get_future().get();
    auto err     = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(updated.has_value());
    REQUIRE(updated->title == "New title");
    REQUIRE(updated->url == "https://new.url");
}

TEST_CASE("ItemsRepository::deleteItem removes item") {
    auto client = makeClient();
    resetDb(client);

    auto created = createItem(client);

    std::promise<AppError> errorPromise;

    ItemsRepository::deleteItem(
            client,
            created.id,
            [&](const AppError& e) {
                errorPromise.set_value(e);
            }
    );

    auto err = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());

    std::promise<std::optional<ItemDTO>> itemPromise;

    ItemsRepository::getItemById(
            client,
            created.id,
            [&](const std::optional<ItemDTO>& i, const AppError&) {
                itemPromise.set_value(i);
            }
    );

    auto found = itemPromise.get_future().get();

    REQUIRE_FALSE(found.has_value());
}

TEST_CASE("ItemsRepository::listAll respects pagination") {
    auto client = makeClient();
    resetDb(client);

    createItem(client, "1");
    createItem(client, "2");
    createItem(client, "3");

    std::promise<std::vector<ItemDTO>> itemsPromise;
    std::promise<AppError> errorPromise;

    Pagination p {
        .limit = 2,
        .offset = 0
    };

    ItemsRepository::listAll(
            client,
            p,
            [&](const std::vector<ItemDTO>& list, const AppError& e) {
                itemsPromise.set_value(list);
                errorPromise.set_value(e);
            }
    );

    auto items = itemsPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.size() == 2);
}

TEST_CASE("ItemsRepository::getItemsByTagIds returns tagged items") {
    auto client = makeClient();
    resetDb(client);

    auto item1 = createItem(client, "item1");
    auto item2 = createItem(client, "item2");

    int tagA = createTag(client, "tagA");
    int tagB = createTag(client, "tagB");

    linkItemTag(client, item1.id, tagA);
    linkItemTag(client, item2.id, tagB);

    std::promise<std::vector<ItemDTO>> itemsPromise;
    std::promise<AppError> errorPromise;

    ItemsRepository::getItemsByTagIds(
            client,
            {tagA},
            [&](const std::vector<ItemDTO>& list, const AppError& e) {
                itemsPromise.set_value(list);
                errorPromise.set_value(e);
            }
    );

    auto items = itemsPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.size() == 1);
    REQUIRE(items[0].id == item1.id);
}