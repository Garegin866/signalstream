#include "ItemsController.h"

#include "services/ItemsService.h"
#include "core/Response.h"
#include "core/RequestContextHelpers.h"

#include "mappers/MapperRegistry.h"
#include "mappers/ItemMapper.h"

#include <json/json.h>

void ItemsController::createItem(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb
) {
    REQUIRE_ADMIN(req, cb);

    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_TITLE)) {
        cb(jsonError(400, "title required"));
        return;
    }

    std::string title = (*json)[Const::JSON_TITLE].asString();
    std::string description = json->get(Const::JSON_DESC, "").asString();
    std::string url = json->get(Const::JSON_URL, "").asString();

    ItemsService::createItem(
            title, description, url,
            [cb](const ItemDTO& item, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                auto& M = MapperRegistry<ItemDTO, ItemMapper>::get();
                cb(jsonOK(M.toJson(item)));
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
                    cb(makeErrorResponse(err));
                    return;
                }

                if (!item) {
                    cb(jsonError(404, "Item not found"));
                    return;
                }

                auto& M = MapperRegistry<ItemDTO, ItemMapper>::get();
                cb(jsonOK(M.toJson(*item)));
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
                    cb(makeErrorResponse(err));
                    return;
                }

                auto& M = MapperRegistry<ItemDTO, ItemMapper>::get();
                Json::Value arr(Json::arrayValue);
                for (const auto& item : items) {
                    arr.append(M.toJson(item));
                }

                Json::Value data;
                data[Const::JSON_ITEMS] = arr;

                cb(jsonOK(data));
            }
    );
}


void ItemsController::updateItem(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    REQUIRE_ADMIN(req, cb);

    auto json = req->getJsonObject();
    if (!json) {
        cb(jsonError(400, "Invalid json"));
        return;
    }

    std::string title = json->get(Const::JSON_TITLE, "").asString();
    std::string description = json->get(Const::JSON_DESC, "").asString();
    std::string url = json->get(Const::JSON_URL, "").asString();

    ItemsService::updateItem(
            itemId, title, description, url,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                if (!item) {
                    cb(jsonError(404, "Item not found"));
                    return;
                }

                auto& M = MapperRegistry<ItemDTO, ItemMapper>::get();
                cb(jsonOK(M.toJson(*item)));
            }
    );
}


void ItemsController::deleteItem(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    REQUIRE_ADMIN(req, cb);

    ItemsService::deleteItem(
            itemId,
            [cb](bool ok, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                Json::Value r;
                r[Const::JSON_DELETED] = ok;

                cb(jsonOK(r));
            }
    );
}