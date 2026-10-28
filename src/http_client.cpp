#include "http_client.hpp"

#include <curl/curl.h>

#include <memory>
#include <stdexcept>

namespace livestream {

namespace {

// Ограничение размера ответа, как в исходной C-версии
constexpr std::size_t kMaxResponseSize = 4 * 1024 * 1024;

struct CurlDeleter {
    void operator()(CURL* handle) const { curl_easy_cleanup(handle); }
};
using CurlHandle = std::unique_ptr<CURL, CurlDeleter>;

// Колбэк libcurl: дописывает полученный кусок в std::string
std::size_t write_callback(char* ptr, std::size_t size, std::size_t nmemb, void* userdata) {
    auto* body = static_cast<std::string*>(userdata);
    const std::size_t chunk = size * nmemb;
    if (body->size() + chunk > kMaxResponseSize) {
        return 0;  // прерывает передачу с ошибкой CURLE_WRITE_ERROR
    }
    body->append(ptr, chunk);
    return chunk;
}

}  // namespace

CurlGlobal::CurlGlobal() {
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        throw std::runtime_error("не удалось инициализировать libcurl");
    }
}

CurlGlobal::~CurlGlobal() { curl_global_cleanup(); }

std::string http_get(const std::string& url) {
    CurlHandle curl(curl_easy_init());
    if (!curl) {
        throw std::runtime_error("curl_easy_init() вернул NULL");
    }

    std::string body;
    char error[CURL_ERROR_SIZE] = {};

    curl_easy_setopt(curl.get(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl.get(), CURLOPT_ERRORBUFFER, error);
    curl_easy_setopt(curl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl.get(), CURLOPT_WRITEDATA, &body);

    const CURLcode rc = curl_easy_perform(curl.get());
    if (rc != CURLE_OK) {
        throw std::runtime_error(std::string("ошибка запроса: ") +
                                 (error[0] ? error : curl_easy_strerror(rc)));
    }

    long status = 0;
    curl_easy_getinfo(curl.get(), CURLINFO_RESPONSE_CODE, &status);
    if (status >= 400) {
        throw std::runtime_error("HTTP " + std::to_string(status) + ": " + body);
    }
    return body;
}

std::string url_escape(const std::string& value) {
    CurlHandle curl(curl_easy_init());
    if (!curl) {
        throw std::runtime_error("curl_easy_init() вернул NULL");
    }
    char* escaped = curl_easy_escape(curl.get(), value.c_str(), static_cast<int>(value.size()));
    if (!escaped) {
        throw std::runtime_error("curl_easy_escape() вернул NULL");
    }
    std::string result(escaped);
    curl_free(escaped);
    return result;
}

}  // namespace livestream
