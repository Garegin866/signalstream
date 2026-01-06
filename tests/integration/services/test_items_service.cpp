#include <catch2/catch_test_macros.hpp>

#include <future>
#include <optional>
#include <string>
#include <vector>

#include "services/ItemsService.h"
#include "core/AppError.h"
#include "dto/ItemDTO.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

namespace {

    int countRows(const drogon::orm::DbClientPtr& c, const std::string& sql) {
        auto r = c->execSqlSync(sql);
        REQUIRE_FALSE(r.empty());
        return r[0][0].as<int>();
    }

    ItemDTO fetchItemSync(const drogon::orm::DbClientPtr& c, int id) {
        auto r = c->execSqlSync(
                "SELECT id, title, description, url FROM items WHERE id=$1 LIMIT 1;",
                id
        );
        REQUIRE_FALSE(r.empty());
        ItemDTO dto;
        dto.id = r[0]["id"].as<int>();
        dto.title = r[0]["title"].as<std::string>();
        dto.description = r[0]["description"].as<std::string>();
        dto.url = r[0]["url"].as<std::string>();
        return dto;
    }

} // namespace

TEST_CASE("ItemsService::createItem rejects empty title (Validation)") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    ItemsService service(c);

    std::promise<ItemDTO> itemP;
    std::promise<AppError> errP;

    service.createItem(
            "",                       // title
            "desc",
            "https://a.com",
            {},                       // tagIds
            [&](const ItemDTO& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    (void)itemP.get_future().get();
    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Validation);
}

TEST_CASE("ItemsService::createItem rejects empty url (Validation)") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    ItemsService service(c);

    std::promise<ItemDTO> itemP;
    std::promise<AppError> errP;

    service.createItem(
            "title",
            "desc",
            "",                       // url
            {},
            [&](const ItemDTO& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Validation);
}

TEST_CASE("ItemsService::createItem creates item and returns correct DTO fields") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    ItemsService service(c);

    std::promise<ItemDTO> itemP;
    std::promise<AppError> errP;

    service.createItem(
            "Hello",
            "Desc",
            "https://example.com",
            {},
            [&](const ItemDTO& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(item.id > 0);
    REQUIRE(item.title == "Hello");
    REQUIRE(item.description == "Desc");
    REQUIRE(item.url == "https://example.com");

    auto dbItem = fetchItemSync(c, item.id);
    REQUIRE(dbItem.title == "Hello");
    REQUIRE(dbItem.description == "Desc");
    REQUIRE(dbItem.url == "https://example.com");
}

TEST_CASE("ItemsService::createItem attaches tags and creates notifications for users with those tags") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    // prepare: user + tag + user_tag
    int userId = db::builder::createUser(c, "u@test.com").id;
    int tagId  = db::builder::createTag(c, "cpp").id;
    db::builder::attachTagToUser(c, userId, tagId);

    ItemsService service(c);

    std::promise<ItemDTO> itemP;
    std::promise<AppError> errP;

    service.createItem(
            "Tagged item",
            "Desc",
            "https://t.com",
            {tagId},
            [&](const ItemDTO& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(item.id > 0);

    // item_tags should contain the link
    int links = countRows(c,
                          "SELECT COUNT(*) FROM item_tags WHERE item_id = (SELECT MAX(id) FROM items);"
    );
    REQUIRE(links == 1);

    // notifications should be inserted for that user
    int notifs = countRows(c, "SELECT COUNT(*) FROM notifications;");
    REQUIRE(notifs == 1);
}

TEST_CASE("ItemsService::getItem returns item for existing id") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    // Insert an item directly for predictable state
    auto r = c->execSqlSync(
            "INSERT INTO items (title, description, url) VALUES ($1, $2, $3) RETURNING id;",
            "A", "B", "https://x.com"
    );
    int id = r[0]["id"].as<int>();

    ItemsService service(c);

    std::promise<std::optional<ItemDTO>> itemP;
    std::promise<AppError> errP;

    service.getItem(
            id,
            [&](const std::optional<ItemDTO>& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(item.has_value());
    REQUIRE(item->id == id);
    REQUIRE(item->title == "A");
    REQUIRE(item->description == "B");
    REQUIRE(item->url == "https://x.com");
}

TEST_CASE("ItemsService::getItem returns nullopt for unknown id") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    ItemsService service(c);

    std::promise<std::optional<ItemDTO>> itemP;
    std::promise<AppError> errP;

    service.getItem(
            999999,
            [&](const std::optional<ItemDTO>& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE_FALSE(item.has_value());
}

TEST_CASE("ItemsService::listItems returns items with correct DTO fields and order") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    // Insert items with timestamps (DB default created_at should handle ordering)
    c->execSqlSync("INSERT INTO items (title, description, url) VALUES ('first', 'd1', 'https://1.com');");
    c->execSqlSync("INSERT INTO items (title, description, url) VALUES ('second', 'd2', 'https://2.com');");

    ItemsService service(c);

    Pagination p;
    p.limit = 10;
    p.offset = 0;

    std::promise<std::vector<ItemDTO>> listP;
    std::promise<AppError> errP;

    service.listItems(
            p,
            [&](const std::vector<ItemDTO>& items, const AppError& e) {
                listP.set_value(items);
                errP.set_value(e);
            }
    );

    auto items = listP.get_future().get();
    auto err   = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.size() == 2);

    // ORDER BY created_at DESC => "second" should come first
    REQUIRE(items[0].title == "second");
    REQUIRE_FALSE(items[0].description.empty());
    REQUIRE_FALSE(items[0].url.empty());

    REQUIRE(items[1].title == "first");
}

TEST_CASE("ItemsService::updateItem propagates NotFound from repository") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    ItemsService service(c);

    std::promise<std::optional<ItemDTO>> itemP;
    std::promise<AppError> errP;

    service.updateItem(
            999999,
            std::optional<std::string>{"new"},
            std::nullopt,
            std::nullopt,
            [&](const std::optional<ItemDTO>& it, const AppError& e) {
                itemP.set_value(it);
                errP.set_value(e);
            }
    );

    auto item = itemP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::NotFound);
    REQUIRE_FALSE(item.has_value());
}