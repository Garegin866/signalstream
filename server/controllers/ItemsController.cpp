#include "ItemsController.h"
#include "../services/ItemsService.h"
#include "Response.h"
#include <json/json.h>

void ItemsController::createItem(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb
) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("title")) {
        cb(jsonError(400, "title required"));
        return;
    }

    std::string title = (*json)["title"].asString();
    std::string description = json->get("description", "").asString();
    std::string url = json->get("url", "").asString();

    ItemsService::createItem(
            title, description, url,
            [cb](const ItemDTO& item, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(400, err.message));
                    return;
                }

                Json::Value data;
                data["id"] = item.id;
                data["title"] = item.title;
                data["description"] = item.description;
                data["url"] = item.url;

                cb(jsonOK(data));
            }
    );
}


void ItemsController::getItem(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    ItemsService::getItem(
            itemId,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(500, "Internal error"));
                    return;
                }

                if (!item) {
                    cb(jsonError(404, "Item not found"));
                    return;
                }

                Json::Value data;
                data["id"] = item->id;
                data["title"] = item->title;
                data["description"] = item->description;
                data["url"] = item->url;

                cb(jsonOK(data));
            }
    );
}


void ItemsController::listItems(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb
) {
    ItemsService::listItems(
            [cb](const std::vector<ItemDTO>& items, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(500, "Internal error"));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                for (const auto& item : items) {
                    Json::Value v;
                    v["id"] = item.id;
                    v["title"] = item.title;
                    v["description"] = item.description;
                    v["url"] = item.url;
                    arr.append(v);
                }

                Json::Value data;
                data["items"] = arr;

                cb(jsonOK(data));
            }
    );
}


void ItemsController::updateItem(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    auto json = req->getJsonObject();
    if (!json) {
        cb(jsonError(400, "Invalid json"));
        return;
    }

    std::string title = json->get("title", "").asString();
    std::string description = json->get("description", "").asString();
    std::string url = json->get("url", "").asString();

    ItemsService::updateItem(
            itemId, title, description, url,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(400, err.message));
                    return;
                }

                if (!item) {
                    cb(jsonError(404, "Item not found"));
                    return;
                }

                Json::Value r;
                r["id"] = item->id;
                r["title"] = item->title;
                r["description"] = item->description;
                r["url"] = item->url;

                cb(jsonOK(r));
            }
    );
}


void ItemsController::deleteItem(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    ItemsService::deleteItem(
            itemId,
            [cb](bool ok, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(500, "Internal error"));
                    return;
                }

                Json::Value r;
                r["deleted"] = ok;

                cb(jsonOK(r));
            }
    );
}