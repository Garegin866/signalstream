#include <catch2/catch_test_macros.hpp>

#include <future>
#include <string>
#include <vector>
#include <unordered_set>

#include <drogon/orm/DbClient.h>

#include "services/FeedService.h"
#include "core/AppError.h"
#include "dto/FeedItemDTO.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

namespace {

    std::vector<FeedItemDTO> getFeedSync(FeedService& service, int userId, int limit=50, int offset=0) {
        std::promise<std::vector<FeedItemDTO>> itemsP;
        std::promise<AppError> errP;

        service.getFeed(
                userId,
                Pagination{limit, offset},
                [&](const std::vector<FeedItemDTO>& items, const AppError& e) {
                    itemsP.set_value(items);
                    errP.set_value(e);
                }
        );

        auto items = itemsP.get_future().get();
        auto err   = errP.get_future().get();

        REQUIRE_FALSE(err.hasError());
        return items;
    }

    void assertNoDuplicateIds(const std::vector<FeedItemDTO>& items) {
        std::unordered_set<int> seen;
        for (const auto& it : items) {
            REQUIRE(seen.insert(it.id).second); // fails if duplicate
        }
    }

} // namespace

TEST_CASE("FeedService: user with no tags -> empty feed") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "notags@test.com").id;

    // Create tags/items but don't attach any tags to user
    int tagA = db::builder::createTag(c, "cpp").id;
    int item1 = db::builder::createItem(c, "item1").id;
    db::builder::attachTagToItem(c, item1, tagA);

    FeedService service(c);

    auto feed = getFeedSync(service, userId);

    REQUIRE(feed.empty());
}

TEST_CASE("FeedService: user with single tag -> matching items only") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "single@test.com").id;

    int tagA = db::builder::createTag(c, "tagA").id;
    int tagB = db::builder::createTag(c, "tagB").id;

    // User likes only tagA
    db::builder::attachTagToUser(c, userId, tagA);

    // Items
    int itemA1 = db::builder::createItem(c, "A1").id;
    int itemA2 = db::builder::createItem(c, "A2").id;
    int itemB1 = db::builder::createItem(c, "B1").id;

    db::builder::attachTagToItem(c, itemA1, tagA);
    db::builder::attachTagToItem(c, itemA2, tagA);
    db::builder::attachTagToItem(c, itemB1, tagB);

    FeedService service(c);

    auto feed = getFeedSync(service, userId);

    // Must contain only A1/A2
    assertNoDuplicateIds(feed);
    REQUIRE(feed.size() == 2);

    std::unordered_set<std::string> titles;
    for (auto& x : feed) {
        REQUIRE_FALSE(x.title.empty());
        REQUIRE_FALSE(x.description.empty());
        REQUIRE_FALSE(x.url.empty());
        titles.insert(x.title);
    }

    REQUIRE(titles.count("A1") == 1);
    REQUIRE(titles.count("A2") == 1);
    REQUIRE(titles.count("B1") == 0);
}

TEST_CASE("FeedService: user with multiple tags -> union of matching items, no duplicates") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "multi@test.com").id;

    int tagA = db::builder::createTag(c, "tagA").id;
    int tagB = db::builder::createTag(c, "tagB").id;
    int tagC = db::builder::createTag(c, "tagC").id; // not used by user

    // User likes tagA + tagB
    db::builder::attachTagToUser(c, userId, tagA);
    db::builder::attachTagToUser(c, userId, tagB);

    // Items
    int itemA = db::builder::createItem(c, "ItemA").id;
    int itemB = db::builder::createItem(c, "ItemB").id;
    int itemAB = db::builder::createItem(c, "ItemAB").id; // has BOTH tags -> must appear once
    int itemC = db::builder::createItem(c, "ItemC").id;

    db::builder::attachTagToItem(c, itemA, tagA);
    db::builder::attachTagToItem(c, itemB, tagB);

    db::builder::attachTagToItem(c, itemAB, tagA);
    db::builder::attachTagToItem(c, itemAB, tagB);

    db::builder::attachTagToItem(c, itemC, tagC);

    FeedService service(c);

    auto feed = getFeedSync(service, userId);

    // Should include ItemA, ItemB, ItemAB only.
    assertNoDuplicateIds(feed);

    if (feed.size() != 3) {
        // Make failure readable
        INFO("Expected 3 items. Got " << feed.size());
        for (auto& x : feed) INFO("Feed item: id=" << x.id << " title=" << x.title);
    }
    REQUIRE(feed.size() == 3);

    std::unordered_set<std::string> titles;
    for (auto& x : feed) titles.insert(x.title);

    REQUIRE(titles.count("ItemA") == 1);
    REQUIRE(titles.count("ItemB") == 1);
    REQUIRE(titles.count("ItemAB") == 1);
    REQUIRE(titles.count("ItemC") == 0);
}

TEST_CASE("FeedService: empty feed scenario returns success (no AppError)") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "empty@test.com").id;
    // User has tags but no items linked to them
    int tagA = db::builder::createTag(c, "tagA").id;
    db::builder::attachTagToUser(c, userId, tagA);

    FeedService service(c);

    auto feed = getFeedSync(service, userId);

    REQUIRE(feed.empty());
}