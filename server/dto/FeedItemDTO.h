#pragma once
#include <string>
#include <vector>
#include "TagDTO.h"

struct FeedItemDTO {
    int id = 0;
    std::string title;
    std::string description;
    std::string url;
    std::vector<TagDTO> tags;
};