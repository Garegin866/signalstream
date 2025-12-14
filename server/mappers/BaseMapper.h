#pragma once

#include <json/json.h>
#include <drogon/orm/Row.h>

template <typename DTO>
class BaseMapper {
public:
    virtual ~BaseMapper() = default;

    virtual DTO fromRow(const drogon::orm::Row &row) const = 0;
    virtual Json::Value toJson(const DTO &dto) const = 0;
};