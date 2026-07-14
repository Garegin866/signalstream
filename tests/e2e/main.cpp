#include "HttpClientWrapper.h"
#include "JsonParser.h"

#include <drogon/HttpResponse.h>

#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <thread>

#ifdef __unix__
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

namespace fs = std::filesystem;

namespace {

std::string getEnvOrDefault(const char* name, const std::string& fallback) {
    if (const char* value = std::getenv(name)) {
        return value;
    }
    return fallback;
}

std::optional<fs::path> findRepoRoot(const fs::path& start) {
    fs::path current = start;
    for (int i = 0; i < 6; ++i) {
        if (fs::exists(current / "config" / "config.dev.json")) {
            return current;
        }
        if (!current.has_parent_path()) {
            break;
        }
        current = current.parent_path();
    }
    return std::nullopt;
}

std::optional<fs::path> findServerBinary(const fs::path& exeDir) {
    if (const char* overridePath = std::getenv("SIGNALSTREAM_SERVER_BIN")) {
        fs::path candidate = overridePath;
        if (fs::exists(candidate)) {
            return candidate;
        }
    }

    const fs::path candidates[] = {
        exeDir / "signalstream_server",
        exeDir / ".." / "server" / "signalstream_server",
        exeDir / ".." / ".." / "server" / "signalstream_server",
        exeDir / ".." / ".." / ".." / "server" / "signalstream_server"
    };

    for (const auto& candidate : candidates) {
        if (fs::exists(candidate)) {
            return fs::weakly_canonical(candidate);
        }
    }

    return std::nullopt;
}

bool assertStatus(const drogon::HttpResponsePtr& response, int expected) {
    if (!response) {
        std::cout << "  Expected HTTP " << expected << ", but response was null\n";
        return false;
    }

    const auto actual = static_cast<int>(response->getStatusCode());
    if (actual != expected) {
        std::cout << "  Expected HTTP " << expected << ", got " << actual << "\n";
        return false;
    }

    return true;
}

class ServerProcess {
public:
    explicit ServerProcess(const fs::path& exePath)
        : exeDir_(exePath.parent_path())
        , repoRoot_(findRepoRoot(exeDir_))
        , serverBin_(findServerBinary(exeDir_)) {}

    bool ensureRunning(HttpClientWrapper& client) {
        if (isHealthy(client)) {
            return true;
        }

#ifdef __unix__
        if (!serverBin_) {
            std::cout << "  Server binary not found. Set SIGNALSTREAM_SERVER_BIN.\n";
            return false;
        }

        pid_ = fork();
        if (pid_ == 0) {
            if (repoRoot_) {
                chdir(repoRoot_->c_str());
            }
            execl(serverBin_->c_str(), serverBin_->c_str(), nullptr);
            _exit(1);
        }

        if (pid_ < 0) {
            std::cout << "  Failed to fork server process\n";
            return false;
        }

        startedByUs_ = true;
        return waitUntilHealthy(client, std::chrono::seconds(6));
#else
        std::cout << "  Auto-start is not supported on this platform.\n";
        return false;
#endif
    }

    ~ServerProcess() {
#ifdef __unix__
        if (!startedByUs_ || pid_ <= 0) {
            return;
        }

        kill(pid_, SIGTERM);
        for (int i = 0; i < 20; ++i) {
            int status = 0;
            if (waitpid(pid_, &status, WNOHANG) > 0) {
                return;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        kill(pid_, SIGKILL);
        waitpid(pid_, nullptr, 0);
#endif
    }

private:
    bool isHealthy(HttpClientWrapper& client) {
    auto result = client.get("/health");
    return result.result == drogon::ReqResult::Ok &&
           result.response &&
           result.response->getStatusCode() == drogon::k200OK;
}

    bool waitUntilHealthy(HttpClientWrapper& client, std::chrono::seconds timeout) {
        auto start = std::chrono::steady_clock::now();
        while (std::chrono::steady_clock::now() - start < timeout) {
            if (isHealthy(client)) {
                return true;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(200));
        }
        return false;
    }

    fs::path exeDir_;
    std::optional<fs::path> repoRoot_;
    std::optional<fs::path> serverBin_;
    bool startedByUs_{false};
#ifdef __unix__
    pid_t pid_{-1};
#endif
};

} // namespace

int main(int argc, char** argv) {
    std::cout << "SignalStream E2E Tests\n";

    const std::string baseUrl = getEnvOrDefault("SIGNALSTREAM_BASE_URL", "http://127.0.0.1:8080");
    HttpClientWrapper client(baseUrl);

    fs::path exePath = argc > 0 ? fs::absolute(argv[0]) : fs::current_path();
    ServerProcess server(exePath);

    std::cout << "- Ensuring server is running...\n";
    if (!server.ensureRunning(client)) {
        std::cout << "FAIL: Server not available\n";
        return 1;
    }

    bool allPass = true;

    std::cout << "- GET /health\n";
    auto healthResult = client.get("/health");
    bool healthOk = true;
    if (healthResult.result != drogon::ReqResult::Ok) {
        std::cout << "  Request failed (ReqResult=" << static_cast<int>(healthResult.result) << ")\n";
        healthOk = false;
    }

    if (healthOk) {
        healthOk = assertStatus(healthResult.response, 200);
    }
    if (healthOk) {
        Json::Value json;
        std::string error;
        if (!JsonParser::parse(healthResult.response, &json, &error)) {
            std::cout << "  Failed to parse JSON: " << error << "\n";
            healthOk = false;
        }
    }

    if (healthOk) {
        std::cout << "PASS: GET /health\n";
    } else {
        std::cout << "FAIL: GET /health\n";
    }

    allPass = allPass && healthOk;

    std::cout << (allPass ? "ALL TESTS PASSED\n" : "TESTS FAILED\n");
    return allPass ? 0 : 1;
}
