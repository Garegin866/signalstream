#include "FeedService.h"
#include "../repositories/UserTagsRepository.h"
#include "../repositories/ItemsRepository.h"
#include <drogon/drogon.h>

void FeedService::getFeed(
        int userId,
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    UserTagsRepository::listForUser(
            client,
            userId,
            [client, cb](const std::vector<TagDTO>& tags, const AppError& err) {
                if (err.hasError()) {
                    cb({}, err);
                    return;
                }

                if (tags.empty()) {
                    cb({}, AppError{});
                    return;
                }

                std::vector<int> tagIds;
                tagIds.reserve(tags.size());
                for (auto &t : tags)
                    tagIds.push_back(t.id);

                ItemsRepository::getItemsByTagIds(
                        client,
                        tagIds,
                        [cb](const std::vector<ItemDTO>& items, const AppError& err2) {
                            if (err2.hasError()) {
                                cb({}, err2);
                                return;
                            }

                            cb(items, AppError{});
                        }
                );
            }
    );
}