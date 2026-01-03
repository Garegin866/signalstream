#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class AdminController : public drogon::HttpController<AdminController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AdminController::listUsers, "/admin/users", drogon::Get, AUTH_CHAIN);
        ADD_METHOD_TO(AdminController::setRole, "/admin/set-role", drogon::Post, AUTH_CHAIN);
        ADD_METHOD_TO(AdminController::listModerators, "/admin/moderators", drogon::Get, AUTH_CHAIN);
        ADD_METHOD_TO(AdminController::broadcastEmail, "/admin/email/broadcast", drogon::Post, AUTH_CHAIN);
        ADD_METHOD_TO(AdminController::health, "/admin/health", drogon::Get, PUBLIC);
    METHOD_LIST_END

    static void listUsers(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );

    static void setRole(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );

    static void listModerators(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& callback
    );

    static void health(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );

    static void broadcastEmail(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};