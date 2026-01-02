#pragma once

#define USE_AUTH "TokenMiddleware"
#define USE_USER "UserContextMiddleware"
#define USE_LOG "RequestLoggingMiddleware"

// Shortcut for protected endpoints:
#define AUTH_CHAIN USE_AUTH, USE_USER, USE_LOG

// Shortcut for public endpoints:
#define PUBLIC USE_LOG