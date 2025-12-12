#pragma once
#include <json/value.h>
#include <drogon/HttpResponse.h>

using namespace drogon;

inline HttpResponsePtr jsonOK(const Json::Value &body) {
    auto resp = HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(k200OK);
    return resp;
}

inline HttpResponsePtr jsonCreated(const Json::Value &body) {
    auto resp = HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode(k201Created);
    return resp;
}

inline HttpResponsePtr jsonError(int code, const std::string &msg) {
    Json::Value body(Json::objectValue);
    body["error"] = msg;

    auto resp = HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode((HttpStatusCode)code);
    return resp;
}

inline drogon::HttpResponsePtr makeErrorResponse(const AppError& err) {
    Json::Value json;
    json["error"] = err.message;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(toHttpStatus(err));
    return resp;
}
