#include "ItemsService.h"
#include "../repositories/ItemsRepository.h"
#include <drogon/drogon.h>

// ---------------- VALIDATION HELPERS ----------------

static bool isEmpty(const std::string& s) {
    return s.empty() || s.find_first_not_of(" \t\n\r") == std::string::npos;
}

static AppError validateItemFields(
        const std::string& title,
        const std::string& description,
        const std::string& url
) {
    if (isEmpty(title)) {
        return AppError::Validation("Title is required");
    }
    if (title.size() > 200) {
        return AppError::Validation("Title is too long");
    }
    if (description.size() > 2000) {
        return AppError::Validation("Description is too long");
    }
    if (url.size() > 500) {
        return AppError::Validation("URL is too long");
    }
    return AppError{}; // OK
}


// ---------------- SERVICE IMPLEMENTATION ----------------

void ItemsService::createItem(
        const std::string& title,
        const std::string& description,
        const std::string& url,
        std::function<void(const ItemDTO&, const AppError&)> cb
) {
    // 1. Validate
    AppError err = validateItemFields(title, description, url);
    if (err.hasError()) {
        cb({}, err);
        return;
    }

    // 2. Create
    auto client = drogon::app().getDbClient();
    ItemsRepository::createItem(
            client,
            title,
            description,
            url,
            [cb](const ItemDTO& item, const AppError& repoErr) {
                if (repoErr.hasError()) {
                    cb({}, repoErr);
                    return;
                }
                cb(item, AppError{});
            }
    );
}


void ItemsService::getItem(
        int itemId,
        std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::getItemById(
            client,
            itemId,
            [cb](const std::optional<ItemDTO>& item, const AppError& err) {
                cb(item, err);
            }
    );
}


void ItemsService::updateItem(
        int itemId,
        const std::string& title,
        const std::string& description,
        const std::string& url,
        std::function<void(const std::optional<ItemDTO>&, const AppError&)> cb
) {
    // 1. Validate
    AppError err = validateItemFields(title, description, url);
    if (err.hasError()) {
        cb(std::nullopt, err);
        return;
    }

    // 2. Update
    auto client = drogon::app().getDbClient();
    ItemsRepository::updateItem(
            client,
            itemId,
            title,
            description,
            url,
            [cb](const std::optional<ItemDTO>& item, const AppError& repoErr) {
                cb(item, repoErr);
            }
    );
}


void ItemsService::deleteItem(
        int itemId,
        std::function<void(bool, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::deleteItem(
            client,
            itemId,
            [cb](bool ok, const AppError& err) {
                cb(ok, err);
            }
    );
}


void ItemsService::listItems(
        std::function<void(const std::vector<ItemDTO>&, const AppError&)> cb
) {
    auto client = drogon::app().getDbClient();

    ItemsRepository::listAll(
            client,
            [cb](const std::vector<ItemDTO>& items, const AppError& err) {
                cb(items, err);
            }
    );
}