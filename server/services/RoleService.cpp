#include "RoleService.h"

bool RoleService::isAdmin(const UserDTO& user) {
    return user.role == UserRole::Admin;
}

bool RoleService::isModerator(const UserDTO& u) {
    return u.role == UserRole::Moderator;
}

bool RoleService::isPrivileged(const UserDTO& u) {
    return u.role == UserRole::Admin || u.role == UserRole::Moderator;
}

bool RoleService::canManageUsers(const UserDTO& u) {
    return isAdmin(u);
}

bool RoleService::canManageTags(const UserDTO& user) {
    return isAdmin(user);
}

bool RoleService::canManageItems(const UserDTO& user) {
    return isAdmin(user);
}