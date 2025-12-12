#include "TagsService.h"
#include <drogon/HttpAppFramework.h>
#include "repositories/TagsRepository.h"

void TagsService::createTag(
        const std::string &name,
        std::function<void(const TagDTO&, const AppError&)> cb
) {
    if (name.empty()) {
        cb({}, AppError::Validation("name required"));
        return;
    }

    auto client = drogon::app().getDbClient();
    TagsRepository::createTag(client, name, cb);
}

void TagsService::listTags(
        std::function<void(const std::vector<TagDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();
    TagsRepository::listTags(client, cb);
}