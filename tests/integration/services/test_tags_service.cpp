#include <catch2/catch_test_macros.hpp>

#include <future>
#include <vector>
#include <string>

#include "services/TagsService.h"
#include "core/AppError.h"
#include "dto/TagDTO.h"

#include "tests/integration/db_bootstrap.h"

namespace {

    std::vector<std::string> fetchTagNames(const drogon::orm::DbClientPtr& c) {
        auto r = c->execSqlSync("SELECT name FROM tags ORDER BY name;");
        std::vector<std::string> out;
        for (const auto& row : r) {
            out.push_back(row["name"].as<std::string>());
        }
        return out;
    }

} // namespace

TEST_CASE("TagsService::createTag creates tag") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    TagsService service(client);

    std::promise<TagDTO> tagP;
    std::promise<AppError> errP;

    service.createTag(
            "backend",
            [&](const TagDTO& t, const AppError& e) {
                tagP.set_value(t);
                errP.set_value(e);
            }
    );

    auto tag = tagP.get_future().get();
    auto err = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(tag.id > 0);
    REQUIRE(tag.name == "backend");

    auto tags = fetchTagNames(client);
    REQUIRE(tags.size() == 1);
    REQUIRE(tags[0] == "backend");
}

TEST_CASE("TagsService::createTag rejects empty name") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    TagsService service(client);

    std::promise<TagDTO> tagP;
    std::promise<AppError> errP;

    service.createTag(
            "",
            [&](const TagDTO& t, const AppError& e) {
                tagP.set_value(t);
                errP.set_value(e);
            }
    );

    (void)tagP.get_future().get();
    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Validation);

    auto tags = fetchTagNames(client);
    REQUIRE(tags.empty());
}

TEST_CASE("TagsService::createTag rejects duplicate names") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    TagsService service(client);

    // first insert
    {
        std::promise<TagDTO> tagP;
        std::promise<AppError> errP;

        service.createTag(
                "cpp",
                [&](const TagDTO& t, const AppError& e) {
                    tagP.set_value(t);
                    errP.set_value(e);
                }
        );

        (void)tagP.get_future().get();
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }

    // duplicate insert
    std::promise<TagDTO> tagP2;
    std::promise<AppError> errP2;

    service.createTag(
            "cpp",
            [&](const TagDTO& t, const AppError& e) {
                tagP2.set_value(t);
                errP2.set_value(e);
            }
    );

    (void)tagP2.get_future().get();
    auto err = errP2.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Duplicate);

    auto tags = fetchTagNames(client);
    REQUIRE(tags.size() == 1);
}

TEST_CASE("TagsService::listTags returns ordered tags") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    TagsService service(client);

    // insert tags
    for (const auto& name : {"zeta", "alpha", "middle"}) {
        std::promise<TagDTO> tagP;
        std::promise<AppError> errP;

        service.createTag(
                name,
                [&](const TagDTO& t, const AppError& e) {
                    tagP.set_value(t);
                    errP.set_value(e);
                }
        );

        (void)tagP.get_future().get();
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }

    std::promise<std::vector<TagDTO>> listP;
    std::promise<AppError> errP;

    service.listTags(
            [&](const std::vector<TagDTO>& tags, const AppError& e) {
                listP.set_value(tags);
                errP.set_value(e);
            }
    );

    auto tags = listP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(tags.size() == 3);

    REQUIRE(tags[0].name == "alpha");
    REQUIRE(tags[1].name == "middle");
    REQUIRE(tags[2].name == "zeta");
}