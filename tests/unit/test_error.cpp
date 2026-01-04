#include <catch2/catch_all.hpp>

#include "core/AppError.h"

TEST_CASE("AppError default is no error") {
    AppError e;
    REQUIRE_FALSE(e.hasError());
}

TEST_CASE("AppError factories set fields") {
    auto e1 = AppError::Validation("bad input");
    REQUIRE(e1.hasError());
    REQUIRE(e1.type == ErrorType::Validation);
    REQUIRE(e1.message == "bad input");

    auto e2 = AppError::Database("db down");
    REQUIRE(e2.hasError());
    REQUIRE(e2.type == ErrorType::Database);
    REQUIRE(e2.message == "db down");
}

TEST_CASE("AppError does not silently accept empty messages") {
    auto e = AppError::Forbidden("");
    REQUIRE(e.hasError());
    REQUIRE_FALSE(e.message.empty());
}