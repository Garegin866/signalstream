#pragma once
#include <drogon/drogon.h>
#include "Error.h"
#include "Response.h"
#include "Constants.h"

#include "../dto/AuthDTO.h"


#define REQUIRE_USER(req, fcb)                                                            \
        auto attributes = req->attributes();                                              \
        if (!attributes->find(Const::ATTR_USER_ID)) {                                     \
            fcb(makeErrorResponse(AppError::Unauthorized("Missing userId")));             \
            return;                                                                       \
        }                                                                                 \
        if (!attributes->find(Const::ATTR_USER)) {                                        \
            fcb(makeErrorResponse(AppError::Unauthorized("Missing user object")));        \
            return;                                                                       \
        }

#define REQUIRE_AUTH_USER(req, fcb, userVar)                                              \
    REQUIRE_USER(req, fcb);                                                               \
    auto userVar = req->attributes()->get<UserDTO>(Const::ATTR_USER);

#define REQUIRE_AUTH_USER_WITH_TOKEN(req, fcb, userVar, tokenVar)                         \
    REQUIRE_USER(req, fcb);                                                               \
    auto userVar = req->attributes()->get<UserDTO>(Const::ATTR_USER);                     \
    auto tokenVar = req->attributes()->get<std::string>(Const::ATTR_TOKEN);

#define REQUIRE_ROLE(req, fcb, requiredRole)                                              \
    do {                                                                                  \
        REQUIRE_AUTH_USER(req, fcb, user)                                                 \
        if (user.role != requiredRole) {                                                  \
            fcb(makeErrorResponse(AppError::Unauthorized("Insufficient role")));          \
            return;                                                                       \
        }                                                                                 \
    } while(0)

#define REQUIRE_ADMIN(req, fcb)                                                           \
    do {                                                                                  \
        REQUIRE_AUTH_USER(req, fcb, user)                                                 \
        if (user.role != "admin") {                                                       \
            fcb(makeErrorResponse(AppError::Forbidden("Admin access required")));         \
            return;                                                                       \
        }                                                                                 \
    } while (0)