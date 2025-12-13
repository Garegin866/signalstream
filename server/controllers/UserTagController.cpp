#include "UserTagController.h"
#include "../services/UserTagService.h"
#include "Response.h"
#include "dto/TagDTO.h"
#include "../services/AuthService.h"
#include "core/RequestContextHelpers.h"

void UserTagController::attach(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr&)> &&callback
) {
    using namespace drogon;

    REQUIRE_AUTH_USER(req, callback, user);

    auto json = req->getJsonObject();
    if (!json || !json->isMember("tagId")) {
        callback(jsonError(400, "tagId required"));
        return;
    }

    if (!json->isMember("tagId") || !(*json)["tagId"].isInt()) {
        callback(jsonError(400, "tagId must be an integer"));
        return;
    }

    int tagId = (*json)["tagId"].asInt();

    UserTagService::attachTag(
            user.id,
            tagId,
            [callback](const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value ok;
                ok["ok"] = true;

                callback(jsonOK(ok));
            }
    );
}

void UserTagController::list(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    REQUIRE_AUTH_USER(req, callback, user);

    UserTagService::listUserTags(
            user.id,
            [callback](const std::vector<TagDTO>& tags, const AppError& err) {
                if (err.hasError()) {
                    callback(jsonError(500, "Internal error"));
                    return;
                }

                Json::Value arr(Json::arrayValue);
                for (const auto &tag : tags) {
                    Json::Value item;
                    item["id"] = tag.id;
                    item["name"] = tag.name;
                    arr.append(item);
                }

                Json::Value res;
                res["tags"] = arr;

                callback(jsonOK(res));
            }
    );
}