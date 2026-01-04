#include <catch2/catch_all.hpp>
#include "utils/TokenGenerator.h"

#include <unordered_set>

TEST_CASE("TokenGenerator::secureRandom length and basic properties") {
    SECTION("Generates correct length") {
        auto t = TokenGenerator::secureRandom(48);
        REQUIRE(t.size() == 48);
    }

    SECTION("Non-empty for positive length") {
        auto t = TokenGenerator::secureRandom(1);
        REQUIRE_FALSE(t.empty());
    }

    SECTION("Empty for zero length") {
        auto t = TokenGenerator::secureRandom(0);
        REQUIRE(t.empty());
    }
}

TEST_CASE("TokenGenerator::secureRandom uniqueness sanity check") {
    // Not “proof”, but catches dumb bugs like constant token.
    constexpr int N = 200;
    std::unordered_set<std::string> s;
    s.reserve(N);

    for (int i = 0; i < N; ++i) {
        s.insert(TokenGenerator::secureRandom(32));
    }

    REQUIRE((int)s.size() == N);
}