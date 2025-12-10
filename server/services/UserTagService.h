#pragma once
#include "../core/Error.h"
#include <functional>

class UserTagService {
public:
    static void attachTag(
            int userId,
            int tagId,
            std::function<void(const AppError&)> cb
    );
};