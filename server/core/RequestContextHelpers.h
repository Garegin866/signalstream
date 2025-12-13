#pragma once
#include <drogon/drogon.h>
#include "Error.h"
#include "Response.h"
#include "../dto/AuthDTO.h"

#define REQUIRE_USER(req, fcb)                                                     \
         auto attributes = req->attributes();                                      \
        if (!attributes->find("userId")) {                                         \
            fcb(makeErrorResponse(AppError::Unauthorized("Missing userId")));      \
            return;                                                                \
        }                                                                          \
        if (!attributes->find("user")) {                                           \
            fcb(makeErrorResponse(AppError::Unauthorized("Missing user object"))); \
            return;                                                                \
        }

#define REQUIRE_AUTH_USER(req, fcb, userVar)                           \
    REQUIRE_USER(req, fcb);                                            \
    auto userVar = req->attributes()->get<UserDTO>("user");

#define REQUIRE_AUTH_USER_WITH_TOKEN(req, fcb, userVar, tokenVar)       \
    REQUIRE_USER(req, fcb);                                             \
    auto userVar = req->attributes()->get<UserDTO>("user");             \
    auto tokenVar = req->attributes()->get<std::string>("token");