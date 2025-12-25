#pragma once

#include <cstdint>

struct Pagination {
    int limit;
    int offset;

    static constexpr int DEFAULT_LIMIT = 20;
    static constexpr int MAX_LIMIT = 100;
};