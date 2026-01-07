#include "UserTagService.h"

#include "repositories/TagsRepository.h"
#include "repositories/UserTagsRepository.h"

UserTagService::UserTagService(
        drogon::orm::DbClientPtr client
) : client_(std::move(client)) {}

void UserTagService::attachTag(
        int userId,
        int tagId,
        const std::function<void(const AppError&)>& cb
) {
    TagsRepository::findById(
            client_, tagId,
            [this, userId, tagId, cb](const std::optional<TagDTO>& tag, const AppError& err) {
                if (err.hasError()) {
                    cb(err); // NotFound or Database
                    return;
                }
                if (!tag.has_value()) {
                    cb(AppError::NotFound("Tag not found"));
                    return;
                }

                UserTagsRepository::attach(
                        client_, userId, tagId,
                        [cb](bool, const AppError& err2) {
                            if (err2.hasError()) {
                                cb(err2);
                                return;
                            }
                            cb(AppError{}); // success
                        }
                );
            }
    );
}

void UserTagService::listUserTags(
        int userId,
        const std::function<void(const std::vector<TagDTO>&, const AppError&)>& cb
) {
    UserTagsRepository::listForUser(
            client_,
            userId,
            [cb](const std::vector<TagDTO>& tags, const AppError& err) {
                if (err.hasError()) {
                    cb({}, err);
                    return;
                }
                cb(tags, AppError{});
            }
    );
}