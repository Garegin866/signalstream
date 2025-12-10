#include "UserTagService.h"
#include "../repositories/TagsRepository.h"
#include "../repositories/UserTagsRepository.h"
#include <drogon/drogon.h>

void UserTagService::attachTag(
        int userId,
        int tagId,
        std::function<void(const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    TagsRepository::findById(
            client, tagId,
            [client, userId, tagId, cb](const std::optional<TagDTO>& tag, const AppError& err) {
                if (err.hasError()) {
                    cb(err); // NotFound or Database
                    return;
                }
                if (!tag.has_value()) {
                    cb(AppError{ErrorType::NotFound, "Tag not found"});
                    return;
                }

                UserTagsRepository::attach(
                        client, userId, tagId,
                        [cb](bool ok, const AppError& err2) {
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