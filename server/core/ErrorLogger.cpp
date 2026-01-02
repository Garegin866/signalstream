#include "ErrorLogger.h"

#include "dto/AuthDTO.h"
#include "core/Constants.h"

#include <drogon/drogon.h>

using namespace drogon;

static std::string safeUserId(const HttpRequestPtr& req) {
    if (!req) {
        return "-";
    }

    auto attributes = req->attributes();
    if (attributes->find(Const::ATTR_USER)) {
        const auto& user = req->attributes()->get<UserDTO>(Const::ATTR_USER);
        return std::to_string(user.id);
    }

    return "-";
}

void ErrorLogger::logAppError(
        const AppError& err,
        const HttpRequestPtr& req
) {
    LOG_ERROR
            << "type=app_error"
            << " code=" << static_cast<int>(err.type)
            << " msg=\"" << err.message << "\""
            << " path=" << (req ? req->path() : "-")
            << " method=" << (req ? req->methodString() : "-")
            << " userId=" << safeUserId(req);
}