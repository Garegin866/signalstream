#include "FeedController.h"

#include "services/AuthService.h"
#include "services/FeedService.h"
#include "mappers/MapperRegistry.h"
#include "mappers/FeedMapper.h"
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
        callback(makeErrorResponse(err, req));
        return;
    }

    FeedService::getFeed(
            user.id,
            pagination,
            [callback, pagination](const std::vector<FeedItemDTO>& items, const AppError& err) {
                Json::Value out(Json::arrayValue);

                auto& M = MapperRegistry<FeedItemDTO, FeedMapper>::get();
                for (const auto& item : items) {
                    out.append(M.toJson(item));
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