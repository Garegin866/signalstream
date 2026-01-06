#pragma once

#include <drogon/orm/DbClient.h>

using drogon::orm::DbClientPtr;

namespace db::bootstrap {

    inline DbClientPtr makeClient() {
        static DbClientPtr client;

        if (!client) {
            client = drogon::orm::DbClient::newPgClient(
                    "host=127.0.0.1 port=5432 dbname=signaldb_test user=signaluser password=signalpass",
                    1
            );
        }

        return client;
    }

    inline void resetDb(const DbClientPtr& client) {
        client->execSqlSync("TRUNCATE TABLE users, reset_tokens, sessions, "
                            "items, item_tags, "
                            "tags, user_tags, item_tags, "
                            "notifications RESTART IDENTITY;");
    }

}