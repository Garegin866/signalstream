#include <drogon/drogon.h>

int main() {
    drogon::app()
            .loadConfigFile("config/config.dev.json")
            .run();

    return 0;
}
