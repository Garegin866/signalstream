#include "AuthService.h"

#include "core/PasswordHasher.h"
#include "core/TokenGenerator.h"
#include "repositories/UserRepository.h"
#include "repositories/SessionRepository.h"

#include <drogon/HttpAppFramework.h>

void AuthService::registerUser(
        const std::string &email,
        const std::string &password,
        const std::function<void(const UserDTO&, const AppError&)>& cb
) {
    if (email.empty() || password.empty()) {
        cb({}, AppError::Validation("Email and password required"));
        return;
    }

    auto hash = PasswordHasher::hash(password);
    auto client = drogon::app().getDbClient();

    UserRepository::createUser(client, email, hash, cb);
}

void AuthService::loginUser(
        const std::string &email,
        const std::string &password,
        const std::function<void(const SessionDTO&, const AppError&)>& cb
) {
    if (email.empty() || password.empty()) {
        cb({}, AppError::Validation("Email and password required"));
        return;
    }

    auto client = drogon::app().getDbClient();

    UserRepository::findByEmail(
            client,
            email,
            [password, cb, client](const std::optional<UserDTO> &user, const std::string &hash, const AppError &err) {
                if (err.type != ErrorType::None) {
                    cb({}, err);
                    return;
                }

                if (!user.has_value()) {
                    cb({}, AppError::Unauthorized("Invalid credentials"));
                    return;
                }

                if (!PasswordHasher::verify(password, hash)) {
                    cb({}, AppError::Unauthorized("Invalid credentials"));
                    return;
                }

                auto token = TokenGenerator::generate();

                SessionRepository::createSession(
                        client,
                        user->id,
                        token,
                        cb
                );
            }
    );
}

void AuthService::logout(
        const std::string &token,
        const std::function<void(const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    SessionRepository::deleteByToken(
            client, token,
            [cb](bool ok, const AppError& err) {
                if (err.hasError()) {
                    cb(AppError::Database("Could not delete session"));
                    return;
                }
                cb(AppError{}); // success
            }
    );
}
