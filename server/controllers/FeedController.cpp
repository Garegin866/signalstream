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
                    cb(makeErrorResponse(err));
                    return;
                }

                FeedService::getFeed(
                        user.id,
                        [cb](const std::vector<FeedItemDTO>& items, const AppError& err) {
                            Json::Value out(Json::arrayValue);

                            for (const auto& item : items) {
                                Json::Value v;
                                v["id"] = item.id;
                                v["title"] = item.title;
                                v["description"] = item.description;
                                v["url"] = item.url;

                                Json::Value tags(Json::arrayValue);
                                for (const auto& t : item.tags) {
                                    Json::Value tv;
                                    tv["id"] = t.id;
                                    tv["name"] = t.name;
                                    tags.append(tv);
                                }

                                v["tags"] = tags;
                                out.append(v);
                            }

                            cb(jsonOK(out));
                        }
                );
            }
    );
}