#pragma once

#include "dto/FeedItemDTO.h"
#include "core/AppError.h"
#include "pagination/Pagination.h"

#include <functional>
#include <drogon/orm/DbClient.h>

class FeedService {
public:
    explicit FeedService(drogon::orm::DbClientPtr client);

    void getFeed(
            int userId,
            const Pagination& pagination,
            const std::function<void(const std::vector<FeedItemDTO>&, const AppError&)>& cb
    );

private:
    drogon::orm::DbClientPtr client_;
};