#include "UserRepository.h"

#include "core/Constants.h"
#include "core/RoleUtils.h"

void UserRepository::createUser(
        drogon::orm::DbClientPtr client,
        const std::string &email,
        const std::string &passwordHash,
        std::function<void(const UserDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO users (email, password_hash, role) "
            "VALUES ($1, $2, 'user') "
            "RETURNING id, email, role;",
            [cb](const drogon::orm::Result &r) {
                UserDTO dto;
                dto.id    = r[0][Const::COL_ID].as<int>();
                dto.email = r[0][Const::COL_EMAIL].as<std::string>();
                dto.role  = fromString(r[0][Const::COL_ROLE].as<std::string>());

                cb(dto, AppError{});
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
        drogon::orm::DbClientPtr client,
        const std::string &email,
        std::function<void(const std::optional<UserDTO>&,
                           const std::string &passwordHash,
                           const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, email, password_hash, role "
            "FROM users WHERE email=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, "", AppError{});
                    return;
                }

                UserDTO dto;
                dto.id    = r[0][Const::COL_ID].as<int>();
                dto.email = r[0][Const::COL_EMAIL].as<std::string>();
                dto.role  = fromString(r[0][Const::COL_ROLE].as<std::string>());

                const std::string hash = r[0][Const::COL_PASSWORD_HASH].as<std::string>();
                cb(dto, hash, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, "", AppError::Database("Database error"));
            },
            email
    );
}


void UserRepository::findById(
        drogon::orm::DbClientPtr client,
        int userId,
        std::function<void(const std::optional<UserDTO>&, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, email, role FROM users WHERE id=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, AppError::NotFound("User not found"));
                    return;
                }

                UserDTO dto;
                dto.id    = r[0][Const::COL_ID].as<int>();
                dto.email = r[0][Const::COL_EMAIL].as<std::string>();
                dto.role  = fromString(r[0][Const::COL_ROLE].as<std::string>());

                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr&) {
                cb(std::nullopt, AppError::Database("Database error"));
            },
            userId
    );
}