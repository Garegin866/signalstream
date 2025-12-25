#pragma once

#include "BaseMapper.h"

#include "dto/NotificationDTO.h"
#include "core/Constants.h"

#include <drogon/orm/Row.h>

class NotificationMapper : public BaseMapper<NotificationDTO> {
public:
    [[nodiscard]] NotificationDTO fromRow(const drogon::orm::Row &row) const override {
        NotificationDTO dto;
        dto.id        = row[Const::COL_ID].as<int>();
        dto.userId    = row[Const::COL_USER_ID].as<int>();
        dto.type      = row[Const::COL_TYPE].as<std::string>();
        dto.message   = row[Const::COL_MESSAGE].as<std::string>();
        dto.entityType = row[Const::COL_ENTITY_TYPE].as<std::string>();
        dto.entityId   = row[Const::COL_ENTITY_ID].as<int>();
        dto.createdAt = row[Const::COL_CREATED_AT].as<std::string>();

        if (!row[Const::COL_READ_AT].isNull())
            dto.readAt = row[Const::COL_READ_AT].as<std::string>();

        return dto;
    }

    [[nodiscard]] Json::Value toJson(const NotificationDTO &dto) const override {
        Json::Value body;
//        body[Const::JSON_ID] = dto.id;
//        body[Const::JSON_EMAIL] = dto.email;
//        body[Const::JSON_ROLE] = toString(dto.role);

        return body;
    }
};