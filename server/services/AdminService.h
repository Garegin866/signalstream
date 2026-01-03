#pragma once

#include "dto/AuthDTO.h"
#include "dto/HealthDTO.h"
#include "core/Error.h"

#include <vector>
#include <functional>

class AdminService {
public:
    static void listUsers(
            const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
    );

    static void setRole(int actorId, int targetUserId, UserRole newRole,
            const std::function<void(const UserDTO&, const AppError&)>& cb
    );

    static void listModerators(
            const std::function<void(const std::vector<UserDTO>&, const AppError&)>& cb
    );

    static void health(
            const std::function<void(const HealthDTO&)>& cb
    );

    static void broadcastEmail(
            const std::string& subject,
            const std::string& message,
            const std::function<void(const AppError&)>& cb
    );
};