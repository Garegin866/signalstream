#pragma once
#include "ItemDTO.h"
#include <json/json.h>

inline Json::Value toJson(const ItemDTO& dto) {
    Json::Value v;
    v["id"] = dto.id;
    v["title"] = dto.title;
    return v;
}