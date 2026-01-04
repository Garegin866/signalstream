#pragma once

#include "dto/FeedItemDTO.h"
#include "core/AppError.h"
#include "pagination/Pagination.h"

#include <functional>

class FeedService {
public:
    static void getFeed(
            int userId,
            const Pagination& pagination,
            const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
    );
};