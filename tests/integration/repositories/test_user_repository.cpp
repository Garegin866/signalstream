#include <catch2/catch_test_macros.hpp>

#include <drogon/orm/DbClient.h>

#include "repositories/UserRepository.h"
#include "dto/AuthDTO.h"

#include "tests/integration/db_bootstrap.h"
#include "tests/integration/db_builders.h"

using drogon::orm::DbClientPtr;

TEST_CASE("UserRepository::createUser inserts a user") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    std::promise<UserDTO> userPromise;
    std::promise<AppError> errorPromise;

    UserRepository::createUser(
            client,
            "user1@test.com",
            "password_hash",
            [&](const UserDTO& u, const AppError& e) {
                userPromise.set_value(u);
                errorPromise.set_value(e);
            }
    );

    auto user = userPromise.get_future().get();
    auto err  = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(user.id >= 0);
    REQUIRE(user.email == "user1@test.com");
}

TEST_CASE("UserRepository::findByEmail finds existing user") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    auto created = db::builder::createUser(client, "find@test.com");

    std::promise<std::optional<UserDTO>> resultPromise;
    std::promise<AppError> errorPromise;

    UserRepository::findByEmail(
            client,
            "find@test.com",
            [&](const std::optional<UserDTO>& u,
                const std::string& passwordHash,
                const AppError& e) {
                (void)passwordHash;
                resultPromise.set_value(u);
                errorPromise.set_value(e);
            }
    );

    auto found = resultPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(found.has_value());
    REQUIRE(found->id == created.id);
}

TEST_CASE("UserRepository::findById returns null for unknown id") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    std::promise<std::optional<UserDTO>> resultPromise;
    std::promise<AppError> errorPromise;

    UserRepository::findById(
            client,
            9999,
            [&](const std::optional<UserDTO>& u, const AppError& e) {
                resultPromise.set_value(u);
                errorPromise.set_value(e);
            }
    );

    auto found = resultPromise.get_future().get();
    auto err   = errorPromise.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE_FALSE(found.has_value());

}