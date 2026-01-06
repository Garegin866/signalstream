#include "AuthService.h"

#include "services/EmailService.h"
#include "utils/PasswordHasher.h"
#include "utils/PasswordValidator.h"
#include "utils/TokenGenerator.h"
#include "repositories/UserRepository.h"
#include "repositories/SessionRepository.h"
#include "repositories/ResetTokenRepository.h"
#include "core/TimeConstants.h"

AuthService::AuthService(
        drogon::orm::DbClientPtr client
) : client_(std::move(client)) {}

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

    UserRepository::createUser(client_, email, hash, cb);
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

    UserRepository::findByEmail(
            client_,
            email,
            [this, password, cb](const std::optional<UserDTO> &user, const std::string &hash, const AppError &err) {
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
                        client_,
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
    SessionRepository::deleteByToken(
            client_, token,
            [cb](bool, const AppError& err) {
                if (err.hasError()) {
                    cb(AppError::Database("Could not delete session"));
                    return;
                }
                cb(AppError{});
            }
    );
}

void AuthService::requestPasswordReset(
        const std::string& email,
        std::function<void(const AppError&)>&& cb
) {
    UserRepository::findByEmail(
            client_,
            email,
            [this, cb](const std::optional<UserDTO>& user, const std::string&, const AppError&) {
                if (!user) {
                    cb(AppError{});
                    return;
                }

                std::string token = TokenGenerator::secureRandom(48);
                auto expiresAt = std::chrono::system_clock::now() + std::chrono::minutes(TimeConstants::PASSWORD_RESET_TTL);

                ResetTokenRepository::create(
                        client_,
                        user->id,
                        token,
                        expiresAt,
                        [user, token, cb](const AppError& err) {
                            if (err.hasError()) {
                                cb(AppError{});
                                return;
                            }

                            EmailService::sendPasswordReset(
                                    user->email,
                                    token,
                                    [](const AppError&) {}
                            );

                            cb(AppError{});
                        }
                );
            }
    );
}

void AuthService::resetPassword(
        const std::string& token,
        const std::string& newPassword,
        std::function<void(const AppError&)>&& cb
) {
    if (!PasswordValidator::isStrong(newPassword)) {
        cb(AppError::Validation("Weak password"));
        return;
    }

    ResetTokenRepository::findValid(
            client_,
            token,
            [this, newPassword, token, cb](const std::optional<ResetTokenDTO>& dto, const AppError& err) {
                if (err.hasError()) {
                    cb(err);
                    return;
                }

                if (!dto.has_value()) {
                    cb(AppError::Validation("Invalid or expired reset token"));
                    return;
                }

                std::string hash = PasswordHasher::hash(newPassword);

                UserRepository::updatePassword(
                        client_,
                        dto->userId,
                        hash,
                        [this, token, cb](const AppError& err2) {
                            if (err2.hasError()) {
                                cb(err2);
                                return;
                            }

                            ResetTokenRepository::markUsed(
                                    client_,
                                    token,
                                    [cb](const AppError& err3) {
                                        if (err3.hasError()) {
                                            cb(err3);
                                            return;
                                        }

                                        cb(AppError{});
                                    }
                            );
                        }
                );
            }
    );
}