#pragma once
#include "dto/AuthDTO.h"
#include "core/Error.h"

#include <vector>
#include <functional>

class AdminService {
public:
    static void listUsers(
            std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
    );
};