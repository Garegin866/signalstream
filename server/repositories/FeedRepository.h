#pragma once

#include "core/Error.h"
#include "pagination/Pagination.h"
#include "dto/FeedItemDTO.h"

#include <functional>
#include <vector>

#include <drogon/orm/DbClient.h>

class FeedRepository {
public:
    static void getFeedForUser(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const Pagination& pagination,
            const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
    );

};