#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/UserTagsRepository.h"

#include "dto/TagDTO.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

TEST_CASE("UserTagsRepository::attach attaches tag to user") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    int userId = db::builder::createUser(client, "user1@test.com").id;
    int tagId  = db::builder::createTag(client, "cpp").id;

    std::promise<bool> okP;
    std::promise<AppError> errP;

    UserTagsRepository::attach(
            client,
            userId,
            tagId,
            [&](bool ok, const AppError& e) {
                okP.set_value(ok);
                errP.set_value(e);
            }
    );

    auto res = okP.get_future().get();
    auto err = errP.get_future().get();

    REQUIRE(res);
    REQUIRE_FALSE(err.hasError());
}

TEST_CASE("UserTagsRepository::attach blocks duplicates") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    int userId = db::builder::createUser(client, "user2@test.com").id;
    int tagId  = db::builder::createTag(client, "backend").id;

    // First attach
    {
        std::promise<bool> okP;
        std::promise<AppError> errP;

        UserTagsRepository::attach(
                client, userId, tagId,
                [&](bool ok, const AppError& e) {
                    okP.set_value(ok);
                    errP.set_value(e);
                }
        );

        REQUIRE(okP.get_future().get());
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }

    // Duplicate attach
    {
        std::promise<bool> okP;
        std::promise<AppError> errP;

        UserTagsRepository::attach(
                client, userId, tagId,
                [&](bool ok, const AppError& e) {
                    okP.set_value(ok);
                    errP.set_value(e);
                }
        );

        REQUIRE_FALSE(okP.get_future().get());
        REQUIRE(errP.get_future().get().type == ErrorType::Duplicate);
    }
}

TEST_CASE("UserTagsRepository::listForUser returns correct tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    int userId = db::builder::createUser(client, "user3@test.com").id;
    int tagA   = db::builder::createTag(client, "alpha").id;
    int tagB   = db::builder::createTag(client, "beta").id;

    UserTagsRepository::attach(client, userId, tagA, [](bool, const AppError&){});
    UserTagsRepository::attach(client, userId, tagB, [](bool, const AppError&){});

    std::promise<std::vector<TagDTO>> tagsP;
    std::promise<AppError> errP;

    UserTagsRepository::listForUser(
            client,
            userId,
            [&](const std::vector<TagDTO>& tags, const AppError& e) {
                tagsP.set_value(tags);
                errP.set_value(e);
            }
    );

    auto tags = tagsP.get_future().get();
    REQUIRE_FALSE(errP.get_future().get().hasError());
    REQUIRE(tags.size() == 2);
    REQUIRE(tags[0].name == "alpha");
    REQUIRE(tags[1].name == "beta");
}

TEST_CASE("UserTagsRepository::findUsersByTagIds returns correct users") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    int user1 = db::builder::createUser(client, "u1@test.com").id;
    int user2 = db::builder::createUser(client, "u2@test.com").id;
    int tag   = db::builder::createTag(client, "infra").id;

    UserTagsRepository::attach(client, user1, tag, [](bool, const AppError&){});
    UserTagsRepository::attach(client, user2, tag, [](bool, const AppError&){});

    std::promise<std::vector<int>> usersP;
    std::promise<AppError> errP;

    UserTagsRepository::findUsersByTagIds(
            client,
            {tag},
            [&](const std::vector<int>& users, const AppError& e) {
                usersP.set_value(users);
                errP.set_value(e);
            }
    );

    auto users = usersP.get_future().get();
    REQUIRE_FALSE(errP.get_future().get().hasError());
    REQUIRE(users.size() == 2);
}