#include <catch2/catch_test_macros.hpp>

#include <future>
#include <optional>
#include <string>

#include "services/AuthService.h"
#include "utils/PasswordHasher.h"
#include "core/AppError.h"

#include "tests/integration/db_bootstrap.h"

namespace {

    std::string getStoredHash(const drogon::orm::DbClientPtr& c, const std::string& email) {
        auto r = c->execSqlSync("SELECT password_hash FROM users WHERE email=$1 LIMIT 1;", email);
        REQUIRE_FALSE(r.empty());
        return r[0]["password_hash"].as<std::string>();
    }

    bool sessionExists(const drogon::orm::DbClientPtr& c, const std::string& token) {
        auto r = c->execSqlSync("SELECT id FROM sessions WHERE token=$1 LIMIT 1;", token);
        return !r.empty();
    }

} // namespace

TEST_CASE("AuthService::registerUser creates user and stores hashed password") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    std::promise<UserDTO> userP;
    std::promise<AppError> errP;

    AuthService authService(client);
    authService.registerUser(
            "reg@test.com",
            "StrongPass123!",
            [&](const UserDTO& u, const AppError& e) {
                userP.set_value(u);
                errP.set_value(e);
            }
    );

    auto user = userP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());
    REQUIRE(user.id > 0);
    REQUIRE(user.email == "reg@test.com");

    auto hash = getStoredHash(client, "reg@test.com");
    REQUIRE_FALSE(hash.empty());
    REQUIRE(hash != "StrongPass123!");
    REQUIRE(PasswordHasher::verify("StrongPass123!", hash));
}

TEST_CASE("AuthService::registerUser rejects duplicate email") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    AuthService authService(client);

    {
        std::promise<UserDTO> userP;
        std::promise<AppError> errP;

        authService.registerUser(
                "dup@test.com",
                "StrongPass123!",
                [&](const UserDTO& u, const AppError& e) {
                    userP.set_value(u);
                    errP.set_value(e);
                }
        );

        (void)userP.get_future().get();
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }

    std::promise<UserDTO> userP2;
    std::promise<AppError> errP2;

    authService.registerUser(
            "dup@test.com",
            "StrongPass123!",
            [&](const UserDTO& u, const AppError& e) {
                userP2.set_value(u);
                errP2.set_value(e);
            }
    );

    (void)userP2.get_future().get(); // may be default user
    auto err2 = errP2.get_future().get();

    REQUIRE(err2.hasError());
    REQUIRE(err2.type == ErrorType::Duplicate);
}

TEST_CASE("AuthService::loginUser returns session token for valid credentials") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    AuthService authService(client);
    // register first
    {
        std::promise<UserDTO> userP;
        std::promise<AppError> errP;

        authService.registerUser(
                "login@test.com",
                "StrongPass123!",
                [&](const UserDTO& u, const AppError& e) {
                    userP.set_value(u);
                    errP.set_value(e);
                }
        );

        (void)userP.get_future().get();
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }

    std::promise<SessionDTO> sessP;
    std::promise<AppError> errP;

    authService.loginUser(
            "login@test.com",
            "StrongPass123!",
            [&](const SessionDTO& s, const AppError& e) {
                sessP.set_value(s);
                errP.set_value(e);
            }
    );

    auto sess = sessP.get_future().get();
    auto err  = errP.get_future().get();

    REQUIRE_FALSE(err.hasError());

    // Adjust these fields to your real SessionDTO
    REQUIRE_FALSE(sess.token.empty());
    REQUIRE(sess.userId > 0);

    REQUIRE(sessionExists(client, sess.token));
}

TEST_CASE("AuthService::loginUser rejects wrong password") {
    auto client = db::bootstrap::makeClient();
    db::bootstrap::resetDb(client);

    AuthService authService(client);

    // register first
    {
        std::promise<UserDTO> userP;
        std::promise<AppError> errP;

        authService.registerUser(
                "wrong@test.com",
                "StrongPass123!",
                [&](const UserDTO& u, const AppError& e) {
                    userP.set_value(u);
                    errP.set_value(e);
                }
        );

        (void)userP.get_future().get();
        REQUIRE_FALSE(errP.get_future().get().hasError());
    }


    std::promise<SessionDTO> sessP;
    std::promise<AppError> errP;

    authService.loginUser(
            "wrong@test.com",
            "WrongPass!!!",
            [&](const SessionDTO& s, const AppError& e) {
                sessP.set_value(s);
                errP.set_value(e);
            }
    );

    (void)sessP.get_future().get(); // will be default
    auto err = errP.get_future().get();

    REQUIRE(err.hasError());
    REQUIRE(err.type == ErrorType::Unauthorized);
}