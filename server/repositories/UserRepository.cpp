#include "UserRepository.h"

void UserRepository::createUser(
        drogon::orm::DbClientPtr client,
        const std::string &email,
        const std::string &passwordHash,
        std::function<void(const UserDTO&, const AppError&)> cb
) {
    client->execSqlAsync(
            "INSERT INTO users (email, password_hash) VALUES ($1, $2) RETURNING id, email;",
            [cb](const drogon::orm::Result &r) {
                UserDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.email = r[0]["email"].as<std::string>();
                cb(dto, AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const drogon::orm::DrogonDbException &e) {
                    std::string msg = e.base().what();
                    if (msg.find("duplicate key") != std::string::npos) {
                        cb({}, AppError{ErrorType::Duplicate, "Email already exists"});
                        return;
                    }
                    cb({}, AppError{ErrorType::Database, "Database error"});
                } catch (...) {
                    cb({}, AppError{ErrorType::Unknown, "Internal error"});
                }
            },
            email, passwordHash
    );
}


void UserRepository::findByEmail(
        drogon::orm::DbClientPtr client,
        const std::string &email,
        std::function<void(const std::optional<UserDTO>&, const std::string &, const AppError&)> cb
) {
    client->execSqlAsync(
            "SELECT id, email, password_hash FROM users WHERE email=$1 LIMIT 1;",
            [cb](const drogon::orm::Result &r) {
                if (r.empty()) {
                    cb(std::nullopt, "", AppError{});
                    return;
                }
                UserDTO dto;
                dto.id = r[0]["id"].as<int>();
                dto.email = r[0]["email"].as<std::string>();
                cb(dto, r[0]["password_hash"].as<std::string>(), AppError{});
            },
            [cb](const std::exception_ptr &eptr) {
                cb(std::nullopt, "", AppError{ErrorType::Database, "Database error"});
            },
            email
    );
}
