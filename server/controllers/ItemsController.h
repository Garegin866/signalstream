#pragma once

#include <drogon/HttpController.h>
#include <core/Middleware.h>

class ItemsController : public drogon::HttpController<ItemsController> {
public:
    METHOD_LIST_BEGIN
        ADD_METHOD_TO(ItemsController::getItem, "/items/{1}", drogon::Get, PUBLIC);
        ADD_METHOD_TO(ItemsController::listItems, "/items", drogon::Get, PUBLIC);
        ADD_METHOD_TO(ItemsController::createItem, "/items", drogon::Post, AUTH_CHAIN);
        ADD_METHOD_TO(ItemsController::updateItem, "/items/{1}", drogon::Put, AUTH_CHAIN);
        ADD_METHOD_TO(ItemsController::deleteItem, "/items/{1}", drogon::Delete, AUTH_CHAIN);
    METHOD_LIST_END

    static void createItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    static void getItem(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                 int itemId);

    static void listItems(const drogon::HttpRequestPtr& req,
                   std::function<void(const drogon::HttpResponsePtr&)>&& cb);

    static void updateItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                    int itemId);

    static void deleteItem(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& cb,
                    int itemId);
};