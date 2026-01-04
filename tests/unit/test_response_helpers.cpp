#include <catch2/catch_all.hpp>

#include "core/Response.h"
#include "core/AppError.h"

TEST_CASE("jsonError returns correct status and json") {
    auto resp = jsonError(400, "title required");
    REQUIRE(resp);

    REQUIRE(resp->getStatusCode() == drogon::k400BadRequest);

    auto body = resp->getBody();
    REQUIRE_FALSE(body.empty());

    Json::Value root;
    Json::CharReaderBuilder rb;
    std::string errs;
    std::unique_ptr<Json::CharReader> reader(rb.newCharReader());
    REQUIRE(reader->parse(body.data(), body.data() + body.size(), &root, &errs));
    REQUIRE_FALSE(root.isNull());
}