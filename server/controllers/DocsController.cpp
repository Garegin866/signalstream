#include "DocsController.h"

#include <drogon/HttpResponse.h>

void DocsController::redirect(
        const drogon::HttpRequestPtr&,
        std::function<void(const drogon::HttpResponsePtr&)>&& cb
) {
    auto resp = drogon::HttpResponse::newRedirectionResponse(
            "/openapi/swagger-ui/index.html"
    );

    cb(resp);
}