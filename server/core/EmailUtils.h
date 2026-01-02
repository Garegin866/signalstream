#pragma once

#include <string>
#include <vector>
#include <functional>

namespace ss::utils {

    void sendEmail(
            const std::string& host,
            int port,
            bool useTls,
            const std::string& username,
            const std::string& password,
            const std::string& from,
            const std::vector<std::string>& to,
            const std::string& subject,
            const std::string& body,
            std::function<void(bool, const std::string&)> cb
    );

}