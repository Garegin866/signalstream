#pragma once
#include <functional>
#include "../dto/ItemDTO.h"
#include "../core/Error.h"

class FeedService {
public:
    static void getFeed(
            int userId,
            std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
    );
};