#include "TokenMiddleware.h"
#include "../repositories/SessionRepository.h"
#include "../core/Response.h"

#include <drogon/drogon.h>
#include <drogon/HttpAppFramework.h>

void TokenMiddleware::doFilter(
        const drogon::HttpRequestPtr& req,
        drogon::FilterCallback&& fcb,
        drogon::FilterChainCallback&& fccb
) {

    auto auth = req->getHeader(Const::HEADER_AUTHORIZATION);

    if (auth.empty()) {
        fcb(makeErrorResponse(AppError::Unauthorized("Missing Authorization header"), req));
        return;
    }

    if (auth.rfind(Const::AUTH_BEARER_PREFIX, 0) != 0) {
        fcb(makeErrorResponse(AppError::Unauthorized("Malformed Authorization header"), req));
        return;
    }

    std::string token = auth.substr(7);  // strip “Bearer “

    if (token.empty()) {
        fcb(makeErrorResponse(AppError::Unauthorized("Empty token"), req));
        return;
    }

    auto client = drogon::app().getDbClient();

    SessionRepository::findByToken(
            client,
            token,
            [req, fcb = std::move(fcb), fccb = std::move(fccb)](const std::optional<SessionDTO>& session, const AppError& err) mutable {
                if (err.hasError()) {
                    fcb(makeErrorResponse(err, req));
                    return;
                }

                if (!session) {
                    fcb(makeErrorResponse(AppError::Unauthorized("Invalid or expired token"), req));
                    return;
                }

                // Attach userId to request context
                req->attributes()->insert(Const::ATTR_USER_ID, session->userId);
                req->attributes()->insert(Const::ATTR_TOKEN, session->token);

                // Continue to the next middleware/controller
                fccb();
            }
    );
}