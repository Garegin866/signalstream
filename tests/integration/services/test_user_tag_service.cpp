#include <catch2/catch_test_macros.hpp>

#include <future>
#include <vector>
#include <string>

#include <drogon/orm/DbClient.h>

#include "services/UserTagService.h"
#include "core/AppError.h"
#include "dto/TagDTO.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

namespace {

    int countUserTags(const DbClientPtr& c, int userId) {
        auto r = c->execSqlSync("SELECT COUNT(*) FROM user_tags WHERE user_id=$1;", userId);
        REQUIRE_FALSE(r.empty());
        return r[0][0].as<long long>();
    }

} // namespace

TEST_CASE("UserTagService::attachTag attaches tag to user") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "u1@test.com").id;
    int tagId  = db::builder::createTag(c, "cpp").id;

    UserTagService service(c);

    std::promise<AppError> errP;

    service.attachTag(
            userId,
            tagId,
            [&](const AppError& e) {
                errP.set_value(e);
            }
    );

    auto err = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(countUserTags(c, userId) == 1);
}

TEST_CASE("UserTagService::listUserTags returns tags ordered by name") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "u2@test.com").id;
    int tagA = db::builder::createTag(c, "zeta").id;
    int tagB = db::builder::createTag(c, "alpha").id;
    int tagC = db::builder::createTag(c, "middle").id;

    // Attach directly (sync) to isolate list logic
    c->execSqlSync("INSERT INTO user_tags(user_id, tag_id) VALUES ($1,$2);", userId, tagA);
    c->execSqlSync("INSERT INTO user_tags(user_id, tag_id) VALUES ($1,$2);", userId, tagB);
    c->execSqlSync("INSERT INTO user_tags(user_id, tag_id) VALUES ($1,$2);", userId, tagC);

    UserTagService service(c);

    std::promise<std::vector<TagDTO>> tagsP;
    std::promise<AppError> errP;

    service.listUserTags(
            userId,
            [&](const std::vector<TagDTO>& tags, const AppError& e) {
                tagsP.set_value(tags);
                errP.set_value(e);
            }
    );

    auto tags = tagsP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(tags.size() == 3);

    REQUIRE(tags[0].name == "alpha");
    REQUIRE(tags[1].name == "middle");
    REQUIRE(tags[2].name == "zeta");
}

TEST_CASE("UserTagService::attachTag rejects duplicate attachment") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "u3@test.com").id;
    int tagId  = db::builder::createTag(c, "backend").id;

    // First attach (sync)
    c->execSqlSync("INSERT INTO user_tags(user_id, tag_id) VALUES ($1,$2);", userId, tagId);

    UserTagService service(c);

    std::promise<AppError> errP;

    service.attachTag(
            userId,
            tagId,
            [&](const AppError& e) {
                errP.set_value(e);
            }
    );

    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Duplicate);
    REQUIRE(countUserTags(c, userId) == 1);
}

TEST_CASE("UserTagService::attachTag returns NotFound for nonexistent tag") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int userId = db::builder::createUser(c, "u4@test.com").id;

    UserTagService service(c);

    std::promise<AppError> errP;

    service.attachTag(
            userId,
            999999, // tagId doesn't exist
            [&](const AppError& e) {
                errP.set_value(e);
            }
    );

    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::NotFound);
    REQUIRE(err.message.find("Tag not found") != std::string::npos);
}

TEST_CASE("UserTagService::attachTag invalid user triggers error (requires FK)") {
    auto c = db::bootstrap::makeClient();
    db::bootstrap::resetDb(c);

    int tagId = db::builder::createTag(c, "ops").id;

    UserTagService service(c);

    std::promise<AppError> errP;

    service.attachTag(
            999999, // userId doesn't exist
            tagId,
            [&](const AppError& e) {
                errP.set_value(e);
            }
    );

    auto err = errP.get_future().get();

    // This only works if user_tags.user_id has a FOREIGN KEY to users.id
    // If you don't have FK, this will (wrongly) succeed.
    REQUIRE(err.hasError());
    // Your UserTagsRepository::attach maps generic DB errors to Database(),
    // so FK violation should land here.
    REQUIRE(err.type == ErrorType::Database);
}