#pragma once

#include <functional>
#include <vector>
#include <drogon/orm/DbClient.h>
#include "../core/Error.h"
#include "../dto/FeedItemDTO.h"

class FeedRepository {
public:
    static void getFeedForUser(
            const drogon::orm::DbClientPtr& client,
            int userId,
            const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
    );

};