#pragma once
#include <functional>
#include "../core/Error.h"
#include "../dto/TagDTO.h"
#include "../dto/ItemDTO.h"

class ItemTagService {
public:
    static void attachTag(
            int itemId,
            int tagId,
            std::function<void(bool, const AppError&)> cb
    );

    static void getTagsForItem(
            int itemId,
            std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
    );

    static void removeTag(
            int itemId,
            int tagId,
            std::function<void(bool, const AppError&)> cb
    );
};