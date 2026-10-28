#pragma once

#include <string>

namespace livestream {

// RAII-обёртка над глобальной инициализацией libcurl.
// Объект должен жить всё время работы с HTTP.
class CurlGlobal {
public:
    CurlGlobal();
    ~CurlGlobal();
    CurlGlobal(const CurlGlobal&) = delete;
    CurlGlobal& operator=(const CurlGlobal&) = delete;
};

// Выполняет GET-запрос и возвращает тело ответа.
// При ошибке сети или HTTP-коде >= 400 бросает std::runtime_error.
std::string http_get(const std::string& url);

// Экранирует строку для подстановки в query-параметр URL.
std::string url_escape(const std::string& value);

}  // namespace livestream
