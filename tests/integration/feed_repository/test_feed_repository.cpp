#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/FeedRepository.h"
#include "repositories/UserRepository.h"
#include "repositories/ItemsRepository.h"
#include "repositories/TagsRepository.h"
#include "repositories/ItemTagsRepository.h"
#include "repositories/UserTagsRepository.h"

#include "dto/FeedItemDTO.h"
#include "dto/AuthDTO.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"

using drogon::orm::DbClientPtr;

namespace {

    UserDTO createUser(const DbClientPtr& c, const std::string& email) {
        std::promise<UserDTO> userPromise;
        std::promise<AppError> errPromise;

        UserRepository::createUser(
                c,
                email,
                "hash",
                [&](const UserDTO& u, const AppError& e) {
                    userPromise.set_value(u);
                    errPromise.set_value(e);
                }
        );

        auto user = userPromise.get_future().get();
        auto err  = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
        return user;
    }

    int createTag(const DbClientPtr& c, const std::string& name) {
        std::promise<TagDTO> tagPromise;
        std::promise<AppError> errPromise;

        TagsRepository::createTag(
                c,
                name,
                [&](const TagDTO& t, const AppError& e) {
                    tagPromise.set_value(t);
                    errPromise.set_value(e);
                }
        );

        auto tag = tagPromise.get_future().get();
        auto err = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
        return tag.id;
    }

    ItemDTO createItem(const DbClientPtr& c, const std::string& title) {
        std::promise<ItemDTO> itemPromise;
        std::promise<AppError> errPromise;

        ItemsRepository::createItem(
                c,
                title,
                "desc",
                "url",
                [&](const ItemDTO& i, const AppError& e) {
                    itemPromise.set_value(i);
                    errPromise.set_value(e);
                }
        );

        auto item = itemPromise.get_future().get();
        auto err  = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
        return item;
    }

} // namespace

// ------------------------------------------------------------
// Tests
// ------------------------------------------------------------

TEST_CASE("FeedRepository::getFeedForUser returns empty feed for user with no tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto user = createUser(client, "empty@feed.com");

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

    auto user = createUser(client, "feed@test.com");

    int tagCpp = createTag(client, "cpp");
    int tagGo  = createTag(client, "go");

    auto item1 = createItem(client, "C++ article");
    auto item2 = createItem(client, "Go article");
    auto item3 = createItem(client, "Rust article");

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

    auto user = createUser(client, "multi@feed.com");

    int tagA = createTag(client, "backend");
    int tagB = createTag(client, "devops");

    auto item = createItem(client, "Infra article");

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