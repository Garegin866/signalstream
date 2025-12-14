#pragma once

#include "core/Middleware.h"

#include <drogon/HttpController.h>

class AdminController : public drogon::HttpController<AdminController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(AdminController::listUsers,
                      "/admin/users",
                      drogon::Get,
                      AUTH_CHAIN);
    METHOD_LIST_END

    void listUsers(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};