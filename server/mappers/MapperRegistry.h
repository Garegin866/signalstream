#pragma once

#include "BaseMapper.h"

#include <type_traits>

template <typename DTO, typename Mapper>
class MapperRegistry {
public:
    static Mapper& get() {
        static Mapper instance;
        return instance;
    }
};