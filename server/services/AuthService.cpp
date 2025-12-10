#include "AuthService.h"
#include "../core/PasswordHasher.h"
#include "../core/TokenGenerator.h"
#include "../repositories/UserRepository.h"
#include "../repositories/SessionRepository.h"
#include <drogon/HttpAppFramework.h>

void AuthService::registerUser(
        const std::string &email,
        const std::string &password,
        std::function<void(const UserDTO&, const AppError&)> cb
) {
    if (email.empty() || password.empty()) {
        cb({}, AppError{ErrorType::Validation, "Email and password required"});
        return;
    }

    auto hash = PasswordHasher::hash(password);
    auto client = drogon::app().getDbClient();

    UserRepository::createUser(client, email, hash, cb);
}

void AuthService::loginUser(
        const std::string &email,
        const std::string &password,
        std::function<void(const SessionDTO&, const AppError&)> cb
) {
    if (email.empty() || password.empty()) {
        cb({}, AppError{ErrorType::Validation, "Email and password required"});
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
                    cb({}, AppError{ErrorType::Unauthorized, "Invalid credentials"});
                    return;
                }

                if (!PasswordHasher::verify(password, hash)) {
                    cb({}, AppError{ErrorType::Unauthorized, "Invalid credentials"});
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

void AuthService::me(
        const std::string &token,
        std::function<void(const UserDTO&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    SessionRepository::findByToken(
            client, token,
            [client, cb](const std::optional<SessionDTO>& session, const AppError &err) {
                if (err.hasError()) {
                    cb({}, AppError{ErrorType::Unauthorized, "Invalid or expired session"});
                    return;
                }
                if (!session.has_value()) {
                    cb({}, AppError{ErrorType::Unauthorized, "Invalid session"});
                    return;
                }

                int userId = session->userId;

                UserRepository::findById(
                        client, userId,
                        [cb](const std::optional<UserDTO>& user, const AppError& err2) {
                            if (err2.hasError()) {
                                cb({}, AppError{ErrorType::Database, "Database error"});
                                return;
                            }
                            if (!user.has_value()) {
                                cb({}, AppError{ErrorType::Unauthorized, "User not found"});
                                return;
                            }

                            cb(user.value(), AppError{});
                        }
                );
            }
    );
}
