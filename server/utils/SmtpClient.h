#pragma once

#include <string>
#include <vector>
#include <functional>

namespace utils {

    struct SmtpConfig {
        std::string host;
        int port = 587;
        bool useTls = true;
        std::string username;
        std::string password;
        std::string from;
    };

    class SmtpClient {
    public:
        static void send(
                const SmtpConfig& cfg,
                const std::vector<std::string>& to,
                const std::string& subject,
                const std::string& body,
                std::function<void(bool success, const std::string& error)> cb
        );
    };

} // namespace utils