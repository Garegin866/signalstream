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

    static void setRole(int targetUserId, UserRole newRole, int actingUserId,
            std::function<void(const UserDTO&, const AppError&)> cb
    );

    static void listModerators(
            std::function<void(const std::vector<UserDTO>&, const AppError&)> cb
    );
};