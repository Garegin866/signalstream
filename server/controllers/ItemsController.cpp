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
    REQUIRE_MOD_OR_ADMIN(req, cb);

    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_TITLE)) {
        cb(jsonError(400, "Title required"));
        return;
    }

    std::string title = (*json)[Const::JSON_TITLE].asString();
    if (title.empty()) {
        cb(jsonError(400, "Title cannot be empty"));
        return;
    }

    std::string description = json->get(Const::JSON_DESC, "").asString();
    std::string url = json->get(Const::JSON_URL, "").asString();

    std::vector<int> tagIds;
    if (json->isMember("tags") && (*json)["tags"].isArray()) {
        for (auto &t : (*json)["tags"]) {
            if (t.isInt()) tagIds.push_back(t.asInt());
        }
    }

    ItemsService::createItem(
            title,
            description,
            url,
            tagIds,
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
    REQUIRE_MOD_OR_ADMIN(req, cb);

    auto json = req->getJsonObject();
    if (!json) {
        cb(jsonError(400, "Invalid json"));
        return;
    }

    std::optional<std::string> title;
    if (json->isMember(Const::JSON_TITLE)) {
        title = (*json)[Const::JSON_TITLE].asString();
    }

    std::optional<std::string> description;
    if (json->isMember(Const::JSON_DESC)) {
        description = (*json)[Const::JSON_DESC].asString();
    }

    std::optional<std::string> url;
    if (json->isMember(Const::JSON_URL)) {
        url = (*json)[Const::JSON_URL].asString();
    }

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
    REQUIRE_MOD_OR_ADMIN(req, cb);

    ItemsService::deleteItem(
            itemId,
            [cb](const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                Json::Value r;
                r[Const::JSON_DELETED] = true;

                cb(jsonOK(r));
            }
    );
}