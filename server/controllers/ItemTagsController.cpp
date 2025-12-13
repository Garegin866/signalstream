#include "ItemTagsController.h"
#include "../services/ItemTagService.h"
#include <json/json.h>
#include "Response.h"

void ItemTagsController::attachTag(
        const drogon::HttpRequestPtr& req,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_TAG_ID)) {
        cb(jsonError(400, "tagId required"));
        return;
    }

    int tagId = (*json)[Const::JSON_TAG_ID].asInt();

    ItemTagService::attachTag(
            itemId,
            tagId,
            [cb](bool ok, const AppError& err) {
                Json::Value r;
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                r[Const::JSON_ATTACHED] = ok;
                cb(jsonOK(r));
            }
    );
}

void ItemTagsController::listTags(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId
) {
    ItemTagService::getTagsForItem(
            itemId,
            [cb](const std::vector<TagDTO>& tags, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                for (const auto& t : tags) {
                    Json::Value v;
                    v[Const::JSON_ID] = t.id;
                    v[Const::JSON_NAME] = t.name;
                    arr.append(v);
                }

                Json::Value r;
                r[Const::JSON_TAGS] = arr;

                cb(jsonOK(r));
            }
    );
}

void ItemTagsController::removeTag(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb,
        int itemId,
        int tagId
) {
    ItemTagService::removeTag(
            itemId,
            tagId,
            [cb](bool ok, const AppError& err) {
                if (err.hasError()) {
                    cb(makeErrorResponse(err));
                    return;
                }

                Json::Value r;
                r[Const::JSON_REMOVED] = ok;
                cb(jsonOK(r));
            }
    );
}