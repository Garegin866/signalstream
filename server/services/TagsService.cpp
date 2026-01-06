#include "TagsService.h"

#include "repositories/TagsRepository.h"

TagsService::TagsService(
        drogon::orm::DbClientPtr client
) : client_(std::move(client)) {}

void TagsService::createTag(
        const std::string &name,
        const std::function<void(const TagDTO&, const AppError&)>& cb
) {
    if (name.empty()) {
        cb({}, AppError::Validation("name required"));
        return;
    }

    TagsRepository::createTag(client_, name, cb);
}

void TagsService::listTags(
        const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
) {
    TagsRepository::listTags(client_, cb);
}