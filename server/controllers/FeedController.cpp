#include "FeedController.h"

#include "services/AuthService.h"
#include "services/FeedService.h"
#include "core/Response.h"
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
                    v[Const::JSON_ID] = item.id;
                    v[Const::JSON_TITLE] = item.title;
                    v[Const::JSON_DESC] = item.description;
                    v[Const::JSON_URL] = item.url;

                    Json::Value tags(Json::arrayValue);
                    for (const auto& t : item.tags) {
                        Json::Value tv;
                        tv[Const::JSON_ID] = t.id;
                        tv[Const::JSON_NAME] = t.name;
                        tags.append(tv);
                    }

                    v[Const::JSON_TAGS] = tags;
                    out.append(v);
                }

                callback(jsonOK(out));
            }
    );
}