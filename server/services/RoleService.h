#pragma once
#include "dto/AuthDTO.h"

class RoleService {
public:
    static bool isAdmin(const UserDTO& user);

    static bool canManageTags(const UserDTO& user);
    static bool canManageItems(const UserDTO& user);
};