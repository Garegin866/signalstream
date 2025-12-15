#pragma once
#include "dto/AuthDTO.h"

class RoleService {
public:
    static bool isAdmin(const UserDTO& user);
    static bool isModerator(const UserDTO& user);
    static bool isPrivileged(const UserDTO& user);

    static bool canManageUsers(const UserDTO& user);
    static bool canManageItems(const UserDTO& user);
    static bool canManageTags(const UserDTO& user);
};