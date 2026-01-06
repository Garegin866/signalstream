#pragma once

#include <drogon/orm/DbClient.h>
#include <catch2/catch_test_macros.hpp>

#include "repositories/UserRepository.h"
#include "repositories/ItemsRepository.h"
#include "repositories/TagsRepository.h"
#include "repositories/ItemTagsRepository.h"

using drogon::orm::DbClientPtr;

namespace db::builder {

    inline UserDTO createUser(
            const DbClientPtr& c,
            const std::string& email
    ) {
        std::promise<UserDTO> userPromise;
        std::promise<AppError> errPromise;

        UserRepository::createUser(
                c,
                email,
                "hash",
                [&](const UserDTO& u, const AppError& e) {
                    userPromise.set_value(u);
                    errPromise.set_value(e);
                }
        );

        auto user = userPromise.get_future().get();
        auto err  = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());

        return user;
    }

    inline TagDTO createTag(
            const DbClientPtr& c,
            const std::string& name
    ) {
        std::promise<TagDTO> tagPromise;
        std::promise<AppError> errPromise;

        TagsRepository::createTag(
                c,
                name,
                [&](const TagDTO& t, const AppError& e) {
                    tagPromise.set_value(t);
                    errPromise.set_value(e);
                }
        );

        auto tag = tagPromise.get_future().get();
        auto err = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());

        return tag;
    }

    inline ItemDTO createItem(
            const DbClientPtr& client,
            const std::string& title = "Title",
            const std::string& desc  = "Desc",
            const std::string& url   = "https://example.com"
    ) {
        std::promise<ItemDTO> itemPromise;
        std::promise<AppError> errorPromise;

        ItemsRepository::createItem(
                client,
                title,
                desc,
                url,
                [&](const ItemDTO& i, const AppError& e) {
                    itemPromise.set_value(i);
                    errorPromise.set_value(e);
                }
        );

        auto result = itemPromise.get_future().get();
        auto err    = errorPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
        REQUIRE(result.id >= 0);

        return result;
    }

    inline void attachTagToItem (
            const DbClientPtr& client,
            int itemId,
            int tagId
    ) {
        std::promise<AppError> errPromise;

        ItemTagsRepository::attachTagsToItem(
                client,
                itemId,
                {tagId},
                [&](bool, const AppError& e) {
                    errPromise.set_value(e);
                }
        );

        auto err = errPromise.get_future().get();

        REQUIRE_FALSE(err.hasError());
    }

} // namespace db::builder