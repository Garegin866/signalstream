#include "FeedService.h"

#include "repositories/FeedRepository.h"

FeedService::FeedService(
        drogon::orm::DbClientPtr client
) : client_(std::move(client)) {}

void FeedService::getFeed(
        int userId,
        const Pagination& pagination,
        const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
) {
    FeedRepository::getFeedForUser(
            client_,
            userId,
            pagination,
            [cb](const std::vector<FeedItemDTO>& items, const AppError& err) {
                cb(items, err);
            }
    );
}