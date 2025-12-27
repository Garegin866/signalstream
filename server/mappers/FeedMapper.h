#pragma once

#include "BaseMapper.h"
#include "dto/FeedItemDTO.h"
#include "core/Constants.h"

class FeedMapper : public BaseMapper<FeedItemDTO> {
public:
    [[nodiscard]] FeedItemDTO fromRow(const drogon::orm::Row &row) const override {
        FeedItemDTO dto;
        dto.id = row[Const::COL_ID].as<int>();
        dto.title = row[Const::COL_TITLE].as<std::string>();
        dto.description = row[Const::COL_DESCRIPTION].as<std::string>();
        dto.url = row[Const::COL_URL].as<std::string>();

        return dto;
    }

    [[nodiscard]] Json::Value toJson(const FeedItemDTO &dto) const override {
        Json::Value body;
        body[Const::JSON_ID] = dto.id;
        body[Const::JSON_TITLE] = dto.title;
        body[Const::JSON_DESC] = dto.description;
        body[Const::JSON_URL] = dto.url;

        Json::Value tags(Json::arrayValue);
        for (const auto& t : dto.tags) {
            Json::Value tv;
            tv[Const::JSON_ID] = t.id;
            tv[Const::JSON_NAME] = t.name;
            tags.append(tv);
        }
        body[Const::JSON_TAGS] = tags;

        return body;
    }
};