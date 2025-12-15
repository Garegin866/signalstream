#pragma once
#include <functional>
#include "../dto/FeedItemDTO.h"
#include "../core/Error.h"

class FeedService {
public:
    static void getFeed(
            int userId,
            const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
    );
};