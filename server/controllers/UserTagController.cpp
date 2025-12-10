#include "UserTagController.h"
#include "../services/UserTagService.h"
#include "Response.h"
#include "dto/TagDTO.h"

#include "../services/AuthService.h"

void UserTagController::attach(
        const drogon::HttpRequestPtr &req,
        std::function<void(const drogon::HttpResponsePtr&)> &&callback
) {
    using namespace drogon;

    auto auth = req->getHeader("Authorization");
    if (auth.size() < 8 || auth.substr(0, 7) != "Bearer ") {
        callback(jsonError(401, "Unauthorized"));
        return;
    }
    std::string token = auth.substr(7);

    AuthService::me(
            token,
            [req, callback](const UserDTO &user, const AppError &authErr) {
                if (authErr.hasError()) {
                    callback(jsonError(401, "Unauthorized"));
                    return;
                }

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
                int userId = user.id;

                UserTagService::attachTag(
                        userId,
                        tagId,
                        [callback](const AppError &err) {
                            if (err.hasError()) {
                                switch (err.type) {
                                    case ErrorType::Duplicate:
                                        callback(jsonError(409, err.message));
                                        return;
                                    case ErrorType::NotFound:
                                        callback(jsonError(404, err.message));
                                        return;
                                    default:
                                        callback(jsonError(500, "Internal error"));
                                        return;
                                }
                            }

                            Json::Value ok;
                            ok["ok"] = true;

                            callback(jsonOK(ok));
                        }
                );
            }
    );
}

void UserTagController::list(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    auto auth = req->getHeader("Authorization");
    if (auth.size() < 8 || auth.substr(0, 7) != "Bearer ") {
        callback(jsonError(401, "Unauthorized"));
        return;
    }

    std::string token = auth.substr(7);

    AuthService::me(
            token,
            [callback](const UserDTO& user, const AppError& authErr) {
                if (authErr.hasError()) {
                    callback(jsonError(401, "Unauthorized"));
                    return;
                }

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
    );
}