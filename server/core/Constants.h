#pragma once
#include <string>

namespace Const {

    // -----------------------------
    // HTTP Header Keys
    // -----------------------------
    inline constexpr const char* HEADER_AUTHORIZATION = "Authorization";
    inline constexpr const char* AUTH_BEARER_PREFIX   = "Bearer ";

    // -----------------------------
    // Attribute Keys (Request Context)
    // -----------------------------
    inline constexpr const char* ATTR_USER_ID = "userId";
    inline constexpr const char* ATTR_USER    = "user";
    inline constexpr const char* ATTR_TOKEN   = "token";

    // -----------------------------
    // JSON Field Keys (Shared)
    // -----------------------------
    inline constexpr const char* JSON_ERROR    = "error";
    inline constexpr const char* JSON_ID       = "id";
    inline constexpr const char* JSON_NAME     = "name";
    inline constexpr const char* JSON_EMAIL    = "email";
    inline constexpr const char* JSON_TITLE    = "title";
    inline constexpr const char* JSON_URL      = "url";
    inline constexpr const char* JSON_DESC     = "description";
    inline constexpr const char* JSON_OK       = "ok";
    inline constexpr const char* JSON_DELETED  = "deleted";
    inline constexpr const char* JSON_REMOVED  = "removed";
    inline constexpr const char* JSON_ATTACHED = "attached";

    // -----------------------------
    // Authentication JSON Keys
    // -----------------------------
    inline constexpr const char* JSON_TOKEN    = "token";
    inline constexpr const char* JSON_PASSWORD = "password";

    // -----------------------------
    // Tag JSON Keys
    // -----------------------------
    inline constexpr const char* JSON_TAG_ID   = "tagId";
    inline constexpr const char* JSON_TAGS     = "tags";

    // -----------------------------
    // Item JSON Keys
    // -----------------------------
    inline constexpr const char* JSON_ITEMS    = "items";

    // -----------------------------
    // DB Column Names
    // -----------------------------
    inline constexpr const char* COL_ID            = "id";
    inline constexpr const char* COL_EMAIL         = "email";
    inline constexpr const char* COL_PASSWORD_HASH = "password_hash";
    inline constexpr const char* COL_NAME          = "name";
    inline constexpr const char* COL_USER_ID       = "user_id";
    inline constexpr const char* COL_TOKEN         = "token";
    inline constexpr const char* COL_TITLE         = "title";
    inline constexpr const char* COL_DESCRIPTION   = "description";
    inline constexpr const char* COL_URL           = "url";
    inline constexpr const char* COL_ITEM_ID       = "item_id";
    inline constexpr const char* COL_TAG_ID        = "tag_id";
    inline constexpr const char* COL_TAG_NAME      = "tag_name";
}