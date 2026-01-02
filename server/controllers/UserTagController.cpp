#include "UserTagController.h"

#include "services/UserTagService.h"
#include "services/AuthService.h"
#include "core/Response.h"
#include "core/RequestContextHelpers.h"
#include "dto/TagDTO.h"

void UserTagController::attach(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr&)> &&callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_TAG_ID)) {
        callback(jsonError(400, "tagId required"));
        return;
    }

    if (!json->isMember(Const::JSON_TAG_ID) || !(*json)[Const::JSON_TAG_ID].isInt()) {
        callback(jsonError(400, "tagId must be an integer"));
        return;
    }

    int tagId = (*json)[Const::JSON_TAG_ID].asInt();

    UserTagService::attachTag(
            user.id,
            tagId,
            [callback, req](const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err, req));
                    return;
                }

                Json::Value ok;
                ok[Const::JSON_OK] = true;

                callback(jsonOK(ok));
            }
    );
}

void UserTagController::list(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    UserTagService::listUserTags(
            user.id,
            [callback, req](const std::vector<TagDTO>& tags, const AppError& err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err, req));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                for (const auto &tag : tags) {
                    Json::Value item;
                    item[Const::JSON_ID] = tag.id;
                    item[Const::JSON_NAME] = tag.name;
                    arr.append(item);
                }

                Json::Value res;
                res[Const::JSON_TAGS] = arr;

                callback(jsonOK(res));
            }
    );
}