#include "UserRepository.h"

#include "core/Constants.h"
#include "mappers/MapperRegistry.h"
#include "mappers/UserMapper.h"

void UserRepository::createUser(
        const drogon::orm::DbClientPtr& client,
        const std::string &email,
        const std::string &passwordHash,
        const std::function<void(const UserDTO&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "INSERT INTO users (email, password_hash, role) "
            "VALUES ($1, $2, 'user') "
            "RETURNING id, email, role;",
            [cb](const drogon::orm::Result &r) {
                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                cb(M.fromRow(r[0]), AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &e) {
                    const std::string msg = e.base().what();

                    if (msg.find("duplicate key") != std::string::npos) {
                        cb({}, AppError::Duplicate("Email already exists"));
                        return;
                    }

                    cb({}, AppError::Database("Database error"));
                } catch (...) {
                    cb({}, AppError::NotFound("Internal error"));
                }
            },
            email, passwordHash
    );
}


void UserRepository::findByEmail(
        const drogon::orm::DbClientPtr& client,
        const std::string &email,
        const std::function<void(const std::optional<UserDTO>&,
                           const std::string &passwordHash,
                           const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, email, password_hash, role "
            "FROM users WHERE email=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, "", AppError{});
                    return;
                }

                auto& M = MapperRegistry<UserDTO, UserMapper>::get();

                const std::string hash = r[0][Const::COL_PASSWORD_HASH].as<std::string>();
                cb(M.fromRow(r[0]), hash, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, "", AppError::Database("Database error"));
            },
            email
    );
}


void UserRepository::findById(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::function<void(const std::optional<UserDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, email, role FROM users WHERE id=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError::NotFound("User not found"));
                    return;
                }

                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                cb(M.fromRow(r[0]), AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError::Database("Database error"));
            },
            userId
    );
}

void UserRepository::listAllUsers(
        const drogon::orm::DbClientPtr& client,
        const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, email, role FROM users ORDER BY id ASC;",
            [cb](const drogon::orm::Result& r) {
                std::vector<UserDTO> users;
                users.reserve(r.size());

                auto& M = MapperRegistry<UserDTO, UserMapper>::get();
                for (const auto& row : r) {
                    users.push_back(M.fromRow(row));
                }

                cb(users, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to list users"));
            }
    );
}

void UserRepository::listActiveEmails(
        const drogon::orm::DbClientPtr& client,
        const std::function<void(const std::vector<std::string>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT email FROM users WHERE email IS NOT NULL;",
            [cb](const drogon::orm::Result& r) {
                std::vector<std::string> emails;
                emails.reserve(r.size());

                for (const auto& row : r) {
                    emails.push_back(row["email"].as<std::string>());
                }

                cb(emails, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to list user emails"));
            }
    );
}

void UserRepository::updateRole(
        const drogon::orm::DbClientPtr& client,
        int userId,
        UserRole role,
        const std::function<void(const UserDTO&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "UPDATE users SET role=$2 WHERE id=$1 RETURNING id, email, role;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb({}, AppError::NotFound("User not found"));
                    return;
                }

                auto M = MapperRegistry<UserDTO, UserMapper>::get();
                cb(M.fromRow(r[0]), AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                cb({}, AppError::Database("Failed to update role"));
            },
            userId,
            toString(role)
    );
}

void UserRepository::listModerators(
        const drogon::orm::DbClientPtr& client,
        const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
) {
    client->execSqlAsync(
            "SELECT id, email, role FROM users WHERE role = 'moderator' ORDER BY id ASC;",
            [cb](const drogon::orm::Result& r) {
                std::vector<UserDTO> out;
                out.reserve(r.size());

                for (const auto& row : r) {
                    auto M = MapperRegistry<UserDTO, UserMapper>::get();
                    out.push_back(M.fromRow(row));
                }

                cb(out, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb({}, AppError::Database("Failed to list moderators"));
            }
    );
}

void UserRepository::updatePassword(
        const drogon::orm::DbClientPtr& client,
        int userId,
        const std::string& passwordHash,
        const std::function<void(const AppError&)>& cb
) {
    client->execSqlAsync(
            "UPDATE users SET password_hash=$2 WHERE id=$1;",
            [cb](const drogon::orm::Result&) {
                cb(AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(AppError::Database("Failed to update password"));
            },
            userId,
            passwordHash
    );
}