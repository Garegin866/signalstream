#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class ItemTagsController : public drogon::HttpController<ItemTagsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ItemTagsController::attachTag, "/items/{1}/tags", drogon::Post, AUTH_CHAIN);
        ADD_METHOD_TO(ItemTagsController::listTags, "/items/{1}/tags", drogon::Get, PUBLIC);
        ADD_METHOD_TO(ItemTagsController::removeTag, "/items/{1}/tags/{2}", drogon::Delete, AUTH_CHAIN);
    METHOD_LIST_END

    static void attachTag(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb,
            int itemId
    );

    static void listTags(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb,
            int itemId
    );

    static void removeTag(
            const drogon::HttpRequestPtr& req,
            std::function<void(const drogon::HttpResponsePtr&)>&& cb,
            int itemId,
            int tagId
    );
};