#include <drogon/drogon.h>

int main() {
    // Optionally load config.json here later.
    // drogon::app().loadConfigFile("config.json");

    // Set basic settings here or via config file later
    drogon::app()
            .addListener("0.0.0.0", 8080) // listen on port 8080
            .run();

    return 0;
}
