#pragma once

#include "BaseMapper.h"
#include "dto/ItemDTO.h"
#include "core/Constants.h"

class ItemMapper : public BaseMapper<ItemDTO> {
public:
    [[nodiscard]] ItemDTO fromRow(const drogon::orm::Row &row) const override {
        ItemDTO dto;
        dto.id    = row["id"].as<int>();
        dto.title = row["title"].as<std::string>();
        dto.description = row["description"].as<std::string>();
        dto.url = row["url"].as<std::string>();
        return dto;
    }

    [[nodiscard]] Json::Value toJson(const ItemDTO &dto) const override {
        Json::Value body;
        body[Const::JSON_ID] = dto.id;
        body[Const::JSON_TITLE] = dto.title;
        body[Const::JSON_DESC] = dto.description;
        body[Const::JSON_URL] = dto.url;

        return body;
    }
};