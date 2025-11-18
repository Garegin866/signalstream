#include "AuthController.h"

#include <drogon/drogon.h>
#include <drogon/utils/Utilities.h>

using namespace drogon;

void AuthController::registerUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    // Parse JSON
    auto json = req->getJsonObject();
    if (!json || !(*json).isMember("email") || !(*json).isMember("password")) {
        Json::Value res;
        res["error"] = "email and password required";
        auto resp = HttpResponse::newHttpJsonResponse(res);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    auto email = (*json)["email"].asString();
    auto password = (*json)["password"].asString();

    // Hash password (simple MD5 for now)
    std::string passwordHash = utils::getMd5(password);

    // Insert into DB
    auto client = app().getDbClient();

    client->execSqlAsync(
            "INSERT INTO users (email, password_hash) VALUES ($1, $2) RETURNING id;",
            // SUCCESS
            [callback](const orm::Result &r) {
                Json::Value res;
                res["status"] = "registered";
                res["user_id"] = r[0]["id"].as<int>();

                auto resp = HttpResponse::newHttpJsonResponse(res);
                resp->setStatusCode(k201Created);
                callback(resp);
            },

            // ERROR
            [callback](const std::exception_ptr &eptr) {
                Json::Value res;

                try {
                    if (eptr) std::rethrow_exception(eptr);
                } catch (const std::exception &e) {
                    std::string msg = e.what();

                    if (msg.find("duplicate key") != std::string::npos) {
                        res["error"] = "email already exists";
                        auto resp = HttpResponse::newHttpJsonResponse(res);
                        resp->setStatusCode(k400BadRequest);
                        callback(resp);
                        return;
                    }

                    res["error"] = msg;
                }

                auto resp = HttpResponse::newHttpJsonResponse(res);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            email, passwordHash
    );
}

void AuthController::loginUser(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("email") || !json->isMember("password")) {
        Json::Value res;
        res["error"] = "email and password required";
        auto resp = HttpResponse::newHttpJsonResponse(res);
        resp->setStatusCode(k400BadRequest);
        callback(resp);
        return;
    }

    auto email = (*json)["email"].asString();
    auto password = (*json)["password"].asString();
    std::string passwordHash = drogon::utils::getMd5(password);

    auto client = drogon::app().getDbClient();

    // Step 1: Find user by email
    client->execSqlAsync(
            "SELECT id, password_hash FROM users WHERE email = $1;",

            // SUCCESS
            [callback, email, passwordHash](const drogon::orm::Result &r) {
                if (r.size() == 0) {
                    Json::Value res;
                    res["error"] = "invalid email or password";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
                    resp->setStatusCode(k401Unauthorized);
                    callback(resp);
                    return;
                }

                int userId = r[0]["id"].as<int>();
                std::string storedHash = r[0]["password_hash"].as<std::string>();

                if (storedHash != passwordHash) {
                    Json::Value res;
                    res["error"] = "invalid email or password";
                    auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
                    resp->setStatusCode(k401Unauthorized);
                    callback(resp);
                    return;
                }

                // Step 2: Create token (simple UUID)
                std::string token = drogon::utils::getUuid();

                // Step 3: Insert session
                auto client2 = drogon::app().getDbClient();
                client2->execSqlAsync(
                        "INSERT INTO sessions (user_id, token) VALUES ($1, $2);",

                        [callback, token](const drogon::orm::Result &result) {
                            Json::Value res;
                            res["token"] = token;

                            auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
                            resp->setStatusCode(k200OK);
                            callback(resp);
                        },

                        [callback](const std::exception_ptr &eptr) {
                            Json::Value res;
                            res["error"] = "failed to create session";
                            auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
                            resp->setStatusCode(k500InternalServerError);
                            callback(resp);
                        },

                        userId, token
                );
            },

            // ERROR
            [callback](const std::exception_ptr &eptr) {
                Json::Value res;
                res["error"] = "database error";
                auto resp = drogon::HttpResponse::newHttpJsonResponse(res);
                resp->setStatusCode(k500InternalServerError);
                callback(resp);
            },
            email
    );
}