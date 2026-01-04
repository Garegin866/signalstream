#pragma once

#include "dto/TagDTO.h"
#include "core/AppError.h"

#include <functional>

class TagsService {
public:
    static void createTag(
            const std::string &name,
            const std::function<void(const TagDTO&, const AppError&)>& cb
    );

    static void listTags(
            const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
    );
};
