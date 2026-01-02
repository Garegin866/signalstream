#include "EmailUtils.h"

#include <curl/curl.h>
#include <sstream>

namespace {

    struct UploadCtx {
        std::string payload;
        size_t offset = 0;
    };

    size_t payloadSource(char* ptr, size_t size, size_t nmemb, void* userp) {
        auto* ctx = static_cast<UploadCtx*>(userp);
        size_t max = size * nmemb;

        if (ctx->offset >= ctx->payload.size())
            return 0;

        size_t len = std::min(max, ctx->payload.size() - ctx->offset);
        memcpy(ptr, ctx->payload.data() + ctx->offset, len);
        ctx->offset += len;
        return len;
    }

} // namespace

namespace utils {

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
    ) {
        CURL* curl = curl_easy_init();
        if (!curl) {
            cb(false, "Failed to init curl");
            return;
        }

        std::ostringstream msg;
        msg << "From: <" << from << ">\r\n";
        for (const auto& r : to)
            msg << "To: <" << r << ">\r\n";
        msg << "Subject: " << subject << "\r\n";
        msg << "\r\n";
        msg << body << "\r\n";

        UploadCtx upload{ msg.str() };

        std::string url =
                (useTls ? "smtps://" : "smtp://") +
                host + ":" + std::to_string(port);

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_USERNAME, username.c_str());
        curl_easy_setopt(curl, CURLOPT_PASSWORD, password.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from.c_str());
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, payloadSource);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        struct curl_slist* recipients = nullptr;
        for (const auto& r : to)
            recipients = curl_slist_append(recipients, r.c_str());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        if (useTls) {
            curl_easy_setopt(curl, CURLOPT_USE_SSL, CURLUSESSL_ALL);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);
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