#pragma once

#include <drogon/HttpController.h>

class DocsController : public drogon::HttpController<DocsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(DocsController::redirect, "/docs", drogon::Get);
    METHOD_LIST_END

    static void redirect(
            const drogon::HttpRequestPtr&,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb
    );
};