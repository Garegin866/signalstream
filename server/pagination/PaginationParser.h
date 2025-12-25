#pragma once

#include "core/Error.h"
#include "Pagination.h"

#include <drogon/HttpRequest.h>

class PaginationParser {
public:
    static Pagination parse(
            const drogon::HttpRequestPtr& req,
            AppError& err
    ) {
        int limit  = Pagination::DEFAULT_LIMIT;
        int offset = 0;

        // limit
        if (req->getParameter("limit").size()) {
            try {
                limit = std::stoi(req->getParameter("limit"));
            } catch (...) {
                err = AppError::Validation("Invalid limit");
                return {};
            }
        }

        // offset
        if (req->getParameter("offset").size()) {
            try {
                offset = std::stoi(req->getParameter("offset"));
            } catch (...) {
                err = AppError::Validation("Invalid offset");
                return {};
            }
        }

        // validation
        if (limit <= 0 || limit > Pagination::MAX_LIMIT) {
            err = AppError::Validation("Limit out of bounds");
            return {};
        }

        if (offset < 0) {
            err = AppError::Validation("Offset must be >= 0");
            return {};
        }

        return Pagination{limit, offset};
    }
};