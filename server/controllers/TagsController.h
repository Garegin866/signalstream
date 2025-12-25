#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class TagsController : public drogon::HttpController<TagsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(TagsController::listTags, "/tags", drogon::Get, PUBLIC);
        ADD_METHOD_TO(TagsController::createTag, "/tags", drogon::Post, AUTH_CHAIN);
    METHOD_LIST_END

    static void listTags(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );

    static void createTag(
            const drogon::HttpRequestPtr &req,
            std::function<void (const drogon::HttpResponsePtr &)> &&callback
    );
};
