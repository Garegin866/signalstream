#include "SmtpClient.h"

#include <curl/curl.h>
#include <sstream>
#include <algorithm>
#include <cstring>

namespace utils {

    static size_t payloadSource(char* ptr, size_t size, size_t nmemb, void* userp) {
        auto* stream = static_cast<std::string*>(userp);
        size_t max = size * nmemb;

        if (stream->empty())
            return 0;

        size_t copy = std::min(max, stream->size());
        memcpy(ptr, stream->c_str(), copy);
        stream->erase(0, copy);
        return copy;
    }

    void SmtpClient::send(
            const SmtpConfig& cfg,
            const std::vector<std::string>& to,
            const std::string& subject,
            const std::string& body,
            std::function<void(bool, const std::string&)> cb
    ) {
        if (cfg.host.empty() || cfg.username.empty() || cfg.password.empty()) {
            cb(false, "SMTP config incomplete");
            return;
        }

        CURL* curl = curl_easy_init();
        if (!curl) {
            cb(false, "curl_easy_init failed");
            return;
        }

        std::ostringstream payload;
        payload << "From: " << cfg.from << "\r\n";
        payload << "To: ";

        for (size_t i = 0; i < to.size(); ++i) {
            payload << to[i];
            if (i + 1 < to.size()) payload << ", ";
        }

        payload << "\r\n";
        payload << "Subject: " << subject << "\r\n";
        payload << "\r\n";
        payload << body << "\r\n";

        std::string payloadStr = payload.str();

        struct curl_slist* recipients = nullptr;
        for (const auto& addr : to)
            recipients = curl_slist_append(recipients, addr.c_str());

        std::string url =
                (cfg.useTls ? "smtp://" : "smtp://") +
                cfg.host + ":" + std::to_string(cfg.port);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, cfg.username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, cfg.password.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, cfg.from.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payloadSource);
        curl_easy_setopt(curl, CURLOPT_READDATA, &payloadStr);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        if (cfg.useTls) {
            curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
        }

        CURLcode res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            cb(false, curl_easy_strerror(res));
        } else {
            cb(true, "");
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

} // namespace utils