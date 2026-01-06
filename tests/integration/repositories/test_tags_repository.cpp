#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include <future>

#include "repositories/TagsRepository.h"
#include "dto/TagDTO.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

TEST_CASE("TagsRepository::createTag inserts a tag") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    std::promise<TagDTO> tagPromise;
    std::promise<AppError> errPromise;

    TagsRepository::createTag(
            client,
            "backend",
            [&](const TagDTO& t, const AppError& e) {
                tagPromise.set_value(t);
                errPromise.set_value(e);
            }
    );

    auto tag = tagPromise.get_future().get();
    auto err = errPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(tag.id > 0);
    REQUIRE(tag.name == "backend");
}

TEST_CASE("TagsRepository::createTag rejects duplicate tag names") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    db::builder::createTag(client, "cpp");

    std::promise<TagDTO> tagPromise;
    std::promise<AppError> errPromise;

    TagsRepository::createTag(
            client,
            "cpp",
            [&](const TagDTO& t, const AppError& e) {
                tagPromise.set_value(t);
                errPromise.set_value(e);
            }
    );

    auto err = errPromise.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Duplicate);
}

TEST_CASE("TagsRepository::listTags returns all tags ordered by name") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    db::builder::createTag(client, "zeta");
    db::builder::createTag(client, "alpha");
    db::builder::createTag(client, "middle");

    std::promise<std::vector<TagDTO>> listPromise;
    std::promise<AppError> errPromise;

    TagsRepository::listTags(
            client,
            [&](const std::vector<TagDTO>& list, const AppError& e) {
                listPromise.set_value(list);
                errPromise.set_value(e);
            }
    );

    auto tags = listPromise.get_future().get();
    auto err  = errPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(tags.size() == 3);

    REQUIRE(tags[0].name == "alpha");
    REQUIRE(tags[1].name == "middle");
    REQUIRE(tags[2].name == "zeta");
}

TEST_CASE("TagsRepository::findById returns existing tag") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto created = db::builder::createTag(client, "infra");

    std::promise<std::optional<TagDTO>> tagPromise;
    std::promise<AppError> errPromise;

    TagsRepository::findById(
            client,
            created.id,
            [&](const std::optional<TagDTO>& t, const AppError& e) {
                tagPromise.set_value(t);
                errPromise.set_value(e);
            }
    );

    auto found = tagPromise.get_future().get();
    auto err   = errPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(found.has_value());
    REQUIRE(found->id == created.id);
    REQUIRE(found->name == "infra");
}

TEST_CASE("TagsRepository::findById returns NotFound for unknown id") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    std::promise<std::optional<TagDTO>> tagPromise;
    std::promise<AppError> errPromise;

    TagsRepository::findById(
            client,
            99999,
            [&](const std::optional<TagDTO>& t, const AppError& e) {
                tagPromise.set_value(t);
                errPromise.set_value(e);
            }
    );

    auto found = tagPromise.get_future().get();
    auto err   = errPromise.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::NotFound);
    REQUIRE_FALSE(found.has_value());
}