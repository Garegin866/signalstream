#pragma once

#define USE_AUTH "TokenMiddleware"
#define USE_USER "UserContextMiddleware"

// Shortcut for protected endpoints:
#define AUTH_CHAIN USE_AUTH, USE_USER

// Shortcut for public endpoints:
#define PUBLIC ""