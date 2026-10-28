#include "youtube.hpp"

#include "http_client.hpp"

#include <cctype>

namespace livestream {

std::string build_search_url(const std::string& channel_id, const std::string& api_key) {
    return "https://www.googleapis.com/youtube/v3/search"
           "?part=snippet"
           "&channelId=" + url_escape(channel_id) +
           "&eventType=live"
           "&type=video"
           "&key=" + url_escape(api_key);
}

std::optional<std::string> extract_video_id(const std::string& json) {
    static const std::string kKey = "\"videoId\"";

    const std::size_t key_pos = json.find(kKey);
    if (key_pos == std::string::npos) {
        return std::nullopt;
    }

    // Пропуск пробелов и двоеточия между ключом и значением
    std::size_t pos = key_pos + kKey.size();
    while (pos < json.size() && (std::isspace(static_cast<unsigned char>(json[pos])) || json[pos] == ':')) {
        ++pos;
    }
    if (pos >= json.size() || json[pos] != '"') {
        return std::nullopt;
    }

    const std::size_t begin = pos + 1;
    const std::size_t end = json.find('"', begin);
    if (end == std::string::npos) {
        return std::nullopt;
    }
    return json.substr(begin, end - begin);
}

bool is_valid_video_id(const std::string& id) {
    if (id.empty()) {
        return false;
    }
    for (const char c : id) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_' && c != '-') {
            return false;
        }
    }
    return true;
}

std::string watch_url(const std::string& video_id) {
    return "https://www.youtube.com/watch?v=" + video_id;
}

}  // namespace livestream
