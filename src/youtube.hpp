#pragma once

#include <optional>
#include <string>

namespace livestream {

// Формирует URL запроса YouTube Data API v3 для поиска активной трансляции канала.
std::string build_search_url(const std::string& channel_id, const std::string& api_key);

// Извлекает первое значение "videoId" из JSON-ответа API.
// Возвращает std::nullopt, если трансляция не найдена.
std::optional<std::string> extract_video_id(const std::string& json);

// Проверяет, что строка похожа на идентификатор видео YouTube
// (только символы [A-Za-z0-9_-]); защищает от подстановки в shell-команду.
bool is_valid_video_id(const std::string& id);

// Возвращает ссылку вида https://www.youtube.com/watch?v=<id>
std::string watch_url(const std::string& video_id);

}  // namespace livestream
