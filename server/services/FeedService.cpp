#include "FeedService.h"

#include "repositories/FeedRepository.h"

#include <drogon/drogon.h>

void FeedService::getFeed(
        int userId,
        const Pagination& pagination,
        const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
) {
    auto client = drogon::app().getDbClient();

    FeedRepository::getFeedForUser(
            client,
            userId,
            pagination,
            [cb](const std::vector<FeedItemDTO>& items, const AppError& err) {
                cb(items, err);
            }
    );
}