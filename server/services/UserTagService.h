#pragma once

#include "core/Error.h"

#include <functional>

struct TagDTO;

class UserTagService {
public:
    static void attachTag(
            int userId,
            int tagId,
            const std::function<void(const AppError&)>& cb
    );

    static void listUserTags(
            int userId,
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );
};