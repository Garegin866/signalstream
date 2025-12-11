#pragma once
#include <drogon/HttpController.h>

class ItemTagsController : public drogon::HttpController<ItemTagsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ItemTagsController::attachTag, "/items/{1}/tags", drogon::Post);
        ADD_METHOD_TO(ItemTagsController::listTags, "/items/{1}/tags", drogon::Get);
        ADD_METHOD_TO(ItemTagsController::removeTag, "/items/{1}/tags/{2}", drogon::Delete);
    METHOD_LIST_END

    void attachTag(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                   int itemId);

    void listTags(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                  int itemId);

    void removeTag(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                   int itemId,
                   int tagId);
};