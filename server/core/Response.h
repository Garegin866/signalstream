#pragma once
#include <json/value.h>
#include <drogon/HttpResponse.h>

#include "Constants.h"

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
    body[Const::JSON_ERROR] = msg;

    auto resp = HttpResponse::newHttpJsonResponse(body);
    resp->setStatusCode((HttpStatusCode)code);
    return resp;
}

inline drogon::HttpResponsePtr makeErrorResponse(const AppError& err) {
    Json::Value json;
    json[Const::JSON_ERROR] = err.message;

    auto resp = drogon::HttpResponse::newHttpJsonResponse(json);
    resp->setStatusCode(toHttpStatus(err));
    return resp;
}
