#include "UserContextMiddleware.h"
#include "../repositories/UserRepository.h"
#include "../core/Error.h"
#include "../core/Response.h"
#include <drogon/drogon.h>

void UserContextMiddleware::doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
) {

    // Expect userId from TokenMiddleware
    auto attributes = req->attributes();
    if (!attributes->find("userId")) {
        fcb(makeErrorResponse(AppError::Unauthorized("Missing userId attribute")));
        return;
    }

    int userId = req->attributes()->get<int>("userId");
    auto client = drogon::app().getDbClient();

    UserRepository::findById(
            client,
            userId,
            [req, fcb = std::move(fcb), fccb = std::move(fccb)](
                    const std::optional<UserDTO>& user,
                    const AppError& err
            ) mutable {
                if (err.hasError()) {
                    fcb(makeErrorResponse(err));
                    return;
                }

                if (!user) {
                    fcb(makeErrorResponse(AppError::Unauthorized("User not found")));
                    return;
                }

                // Inject full UserDTO into request
                req->attributes()->insert("user", *user);

                // Continue the chain
                fccb();
            }
    );
}