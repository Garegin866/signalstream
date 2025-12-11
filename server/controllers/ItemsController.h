#pragma once
#include <drogon/HttpController.h>

class ItemsController : public drogon::HttpController<ItemsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ItemsController::createItem, "/items", drogon::Post);
        ADD_METHOD_TO(ItemsController::getItem, "/items/{1}", drogon::Get);
        ADD_METHOD_TO(ItemsController::listItems, "/items", drogon::Get);
        ADD_METHOD_TO(ItemsController::updateItem, "/items/{1}", drogon::Put);
        ADD_METHOD_TO(ItemsController::deleteItem, "/items/{1}", drogon::Delete);
    METHOD_LIST_END

    void createItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    void getItem(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                 int itemId);

    void listItems(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    void updateItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                    int itemId);

    void deleteItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                    int itemId);
};