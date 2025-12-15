#include "RoleService.h"

bool RoleService::isAdmin(const UserDTO& user) {
    return user.role == UserRole::Admin;
}

bool RoleService::isModerator(const UserDTO& user) {
    return user.role == UserRole::Moderator;
}

bool RoleService::isPrivileged(const UserDTO& user) {
    return user.role == UserRole::Admin || user.role == UserRole::Moderator;
}

bool RoleService::canManageUsers(const UserDTO& user) {
    return ROLE_AT_LEAST(user, UserRole::Admin);
}

bool RoleService::canManageTags(const UserDTO& user) {
    return ROLE_AT_LEAST(user, UserRole::Admin);
}

bool RoleService::canManageItems(const UserDTO& user) {
    return ROLE_AT_LEAST(user, UserRole::Moderator);
}

bool RoleService::isValidRole(UserRole r) {
    return r == UserRole::User ||
           r == UserRole::Moderator ||
           r == UserRole::Admin;
}