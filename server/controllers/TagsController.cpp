#include "TagsController.h"

#include <drogon/drogon.h>

#include "services/TagsService.h"
#include "core/Response.h"

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
                    obj["id"] = t.id;
                    obj["name"] = t.name;
                    arr.append(obj);
                }

                body["tags"] = arr;

                callback(jsonOK(body));
            }
    );
}

void TagsController::createTag(
        const HttpRequestPtr &req,
        std::function<void(const HttpResponsePtr&)> &&callback
) {
    auto json = req->getJsonObject();
    if (!json || !json->isMember("name")) {
        callback(jsonError(400, "name required"));
        return;
    }

    auto name = (*json)["name"].asString();

    TagsService::createTag(
            name,
            [callback](const TagDTO &dto, const AppError &err) {
                if (err.hasError()) {
                    callback(makeErrorResponse(err));
                    return;
                }

                Json::Value body(Json::objectValue);
                body["id"] = dto.id;
                body["name"] = dto.name;
                callback(jsonCreated(body));
                return;
            }
    );
}
