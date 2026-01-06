#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/FeedRepository.h"
#include "repositories/ItemTagsRepository.h"
#include "repositories/UserTagsRepository.h"

#include "dto/FeedItemDTO.h"
#include "dto/AuthDTO.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

TEST_CASE("FeedRepository::getFeedForUser returns empty feed for user with no tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto user = db::builder::createUser(client, "empty@feed.com");

    std::promise<std::vector<FeedItemDTO>> pItems;
    std::promise<AppError> pErr;

    FeedRepository::getFeedForUser(
            client,
            user.id,
            Pagination{10, 0},
            [&](const std::vector<FeedItemDTO>& items, const AppError& e) {
                pItems.set_value(items);
                pErr.set_value(e);
            }
    );

    auto items = pItems.get_future().get();
    auto err   = pErr.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.empty());
}

TEST_CASE("FeedRepository::getFeedForUser returns items matching user tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto user = db::builder::createUser(client, "feed@test.com");

    int tagCpp = db::builder::createTag(client, "cpp").id;
    int tagGo  = db::builder::createTag(client, "go").id;

    auto item1 = db::builder::createItem(client, "C++ article");
    auto item2 = db::builder::createItem(client, "Go article");

    ItemTagsRepository::attachTagsToItem(client, item1.id, {tagCpp}, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    ItemTagsRepository::attachTagsToItem(client, item2.id, {tagGo}, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    UserTagsRepository::attach(client, user.id, tagCpp, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    std::promise<std::vector<FeedItemDTO>> pItems;
    std::promise<AppError> pErr;

    FeedRepository::getFeedForUser(
            client,
            user.id,
            Pagination{10, 0},
            [&](const std::vector<FeedItemDTO>& items, const AppError& e) {
                pItems.set_value(items);
                pErr.set_value(e);
            }
    );

    auto items = pItems.get_future().get();
    auto err   = pErr.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.size() == 1);
    REQUIRE(items[0].title == "C++ article");
    REQUIRE(items[0].tags.size() == 1);
    REQUIRE(items[0].tags[0].name == "cpp");
}

TEST_CASE("FeedRepository::getFeedForUser supports multiple tags and avoids duplicates") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto user = db::builder::createUser(client, "multi@feed.com");

    int tagA = db::builder::createTag(client, "backend").id;
    int tagB = db::builder::createTag(client, "devops").id;

    auto item = db::builder::createItem(client, "Infra article");

    ItemTagsRepository::attachTagsToItem(client, item.id, {tagA, tagB}, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    UserTagsRepository::attach(client, user.id, tagA, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    UserTagsRepository::attach(client, user.id, tagB, [](bool, const AppError& e) {
        REQUIRE_FALSE(e.hasError());
    });

    std::promise<std::vector<FeedItemDTO>> pItems;
    std::promise<AppError> pErr;

    FeedRepository::getFeedForUser(
            client,
            user.id,
            Pagination{10, 0},
            [&](const std::vector<FeedItemDTO>& items, const AppError& e) {
                pItems.set_value(items);
                pErr.set_value(e);
            }
    );

    auto items = pItems.get_future().get();
    auto err   = pErr.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(items.size() == 1); // NO duplicates
    REQUIRE(items[0].tags.size() == 2);
}