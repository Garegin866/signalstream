#include "FeedController.h"

#include "services/AuthService.h"
#include "services/FeedService.h"
#include "core/Response.h"
#include "core/RequestContextHelpers.h"
#include "pagination/PaginationParser.h"

void FeedController::getFeed(
        const HttpRequestPtr& req,
        std::function<void(const HttpResponsePtr&)>&& callback
) {
    REQUIRE_AUTH_USER(req, callback, user)

    AppError err;
    Pagination pagination = PaginationParser::parse(req, err);
    if (err.hasError()) {
        callback(makeErrorResponse(err));
        return;
    }

    FeedService::getFeed(
            user.id,
            pagination,
            [callback, pagination](const std::vector<FeedItemDTO>& items, const AppError& err) {
                Json::Value out(Json::arrayValue);

                for (const auto& item : items) {
                    Json::Value value;
                    value[Const::JSON_ID] = item.id;
                    value[Const::JSON_TITLE] = item.title;
                    value[Const::JSON_DESC] = item.description;
                    value[Const::JSON_URL] = item.url;

                    Json::Value tags(Json::arrayValue);
                    for (const auto& t : item.tags) {
                        Json::Value tv;
                        tv[Const::JSON_ID] = t.id;
                        tv[Const::JSON_NAME] = t.name;
                        tags.append(tv);
                    }

                    value[Const::JSON_TAGS] = tags;

                    out.append(value);
                }

                Json::Value meta;
                meta[Const::JSON_LIMIT] = pagination.limit;
                meta[Const::JSON_OFFSET] = pagination.offset;

                Json::Value body(Json::objectValue);
                body[Const::JSON_ITEMS] = out;
                body[Const::JSON_META] = meta;

                callback(jsonOK(body));
            }
    );
}