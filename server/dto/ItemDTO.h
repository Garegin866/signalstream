#pragma once
#include <string>
#include <vector>
#include "TagDTO.h"

struct ItemDTO {
    int id = 0;
    std::string title;
    std::string description;
    std::string url;
};