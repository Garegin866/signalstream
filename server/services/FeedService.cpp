#include "FeedService.h"
#include <drogon/drogon.h>
#include "../repositories/FeedRepository.h"

void FeedService::getFeed(
        int userId,
        std::function<void(const std::vector<FeedItemDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    FeedRepository::getFeedForUser(
            client,
            userId,
            [cb](const std::vector<FeedItemDTO>& items, const AppError& err) {
                cb(items, err);
            }
    );
}