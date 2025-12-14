#pragma once

#include "BaseMapper.h"
#include "RoleMapper.h"

#include "dto/AuthDTO.h"
#include "core/Constants.h"

#include <drogon/orm/Row.h>

class UserMapper : public BaseMapper<UserDTO> {
public:
    [[nodiscard]] UserDTO fromRow(const drogon::orm::Row &row) const override {
        UserDTO dto;
        dto.id    = row[Const::COL_ID].as<int>();
        dto.email = row[Const::COL_EMAIL].as<std::string>();
        dto.role  = fromString(row[Const::COL_ROLE].as<std::string>());

        return dto;
    }

    [[nodiscard]] Json::Value toJson(const UserDTO &dto) const override {
        Json::Value body;
        body[Const::JSON_ID] = dto.id;
        body[Const::JSON_EMAIL] = dto.email;
        body[Const::JSON_ROLE] = toString(dto.role);

        return body;
    }
};