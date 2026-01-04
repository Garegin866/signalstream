#include <catch2/catch_all.hpp>
#include "utils/PasswordHasher.h"

TEST_CASE("PasswordHasher hash/verify") {
    SECTION("Hash verifies") {
        std::string pw = "CorrectHorseBatteryStaple!";
        auto h = PasswordHasher::hash(pw);
        REQUIRE_FALSE(h.empty());
        REQUIRE(PasswordHasher::verify(pw, h));
    }

    SECTION("Wrong password fails") {
        std::string pw = "password123";
        auto h = PasswordHasher::hash(pw);
        REQUIRE_FALSE(PasswordHasher::verify("password124", h));
    }

    SECTION("Different hashes for same password (salted)") {
        std::string pw = "same_pw";
        auto h1 = PasswordHasher::hash(pw);
        auto h2 = PasswordHasher::hash(pw);
        REQUIRE(h1 != h2);
        REQUIRE(PasswordHasher::verify(pw, h1));
        REQUIRE(PasswordHasher::verify(pw, h2));
    }

    SECTION("Empty password behavior is defined") {
        auto h = PasswordHasher::hash("");
        REQUIRE(PasswordHasher::verify("", h));
    }
}