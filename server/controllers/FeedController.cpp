#include "FeedController.h"
#include "../services/AuthService.h"
#include "../services/FeedService.h"
#include "Response.h"
#include "dto/ItemDTOJson.h"
#include "core/RequestContextHelpers.h"

void FeedController::getFeed(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    REQUIRE_AUTH_USER(req, callback, user);

    FeedService::getFeed(
            user.id,
            [callback](const std::vector<FeedItemDTO>& items, const AppError& err) {
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

                callback(jsonOK(out));
            }
    );
}