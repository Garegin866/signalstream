#include "RoleService.h"

bool RoleService::isAdmin(const UserDTO& user) {
    return user.role == UserRole::Admin;
}

bool RoleService::canManageTags(const UserDTO& user) {
    return isAdmin(user);
}

bool RoleService::canManageItems(const UserDTO& user) {
    return isAdmin(user);
}