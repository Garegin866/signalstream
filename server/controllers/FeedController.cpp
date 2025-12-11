#include "FeedController.h"
#include "../services/AuthService.h"
#include "../services/FeedService.h"
#include "Response.h"
#include "dto/ItemDTOJson.h"

void FeedController::getFeed(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& cb
) {
    auto auth = req->getHeader("Authorization");
    if (auth.size() < 8 || auth.substr(0, 7) != "Bearer ") {
        cb(jsonError(401, "Unauthorized"));
        return;
    }

    std::string token = auth.substr(7);

    AuthService::me(
            token,
            [cb](const UserDTO& user, const AppError& err) {
                if (err.hasError()) {
                    cb(jsonError(401, "Unauthorized"));
                    return;
                }

                FeedService::getFeed(
                        user.id,
                        [cb](const std::vector<ItemDTO>& items, const AppError& err2) {
                            if (err2.hasError()) {
                                cb(jsonError(500, "Internal error"));
                                return;
                            }

                            Json::Value arr(Json::arrayValue);
                            for (auto &item : items) {
                                arr.append(toJson(item));
                            }

                            Json::Value root;
                            root["items"] = arr;

                            cb(jsonOK(root));
                        }
                );
            }
    );
}