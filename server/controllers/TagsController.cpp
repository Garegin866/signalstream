#include "TagsController.h"

#include <drogon/drogon.h>

#include "services/TagsService.h"
#include "core/Response.h"
#include "core/RequestContextHelpers.h"

using namespace drogon;

void TagsController::listTags(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr &)> &&callback
) {
    TagsService::listTags(
            [callback](const std::vector<TagDTO> &tags, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body(Json::objectValue);
                Json::Value arr(Json::arrayValue);

                for (const auto &t : tags) {
                    Json::Value obj(Json::objectValue);
                    obj[Const::JSON_ID] = t.id;
                    obj[Const::JSON_NAME] = t.name;
                    arr.append(obj);
                }

                body[Const::JSON_TAGS] = arr;

                callback(jsonOK(body));
            }
    );
}

void TagsController::createTag(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    REQUIRE_ADMIN(req, callback);

    auto json = req->getJsonObject();
    if (!json || !json->isMember(Const::JSON_NAME)) {
        callback(jsonError(400, "name required"));
        return;
    }

    auto name = (*json)[Const::JSON_NAME].asString();

    TagsService::createTag(
            name,
            [callback](const TagDTO &dto, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body(Json::objectValue);
                body[Const::JSON_ID] = dto.id;
                body[Const::JSON_NAME] = dto.name;
                callback(jsonCreated(body));
                return;
            }
    );
}
