// Получение ссылки на текущую трансляцию YouTube-канала.
//
// Программа запрашивает YouTube Data API v3, находит активную трансляцию
// канала, записывает ссылку на неё в локальный файл в виде JS-переменной
// и (опционально) дописывает идентификатор видео в файл на удалённом
// сервере через ssh.

#include "http_client.hpp"
#include "youtube.hpp"

#include <array>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
#endif

namespace {

struct Options {
    std::string api_key;
    std::string channel_id;
    std::string output = "/var/www/html/url.php";
    std::string ssh;  // например "uhla@sborka.dev -p 12345"; пусто — шаг пропускается
    std::string remote_output = "/var/www/html/url.php";
};

void print_usage(const char* prog) {
    std::cerr << "Использование: " << prog << " --channel <ID> [--key <API_KEY>] [опции]\n"
              << "\n"
              << "  -c, --channel <ID>      идентификатор YouTube-канала\n"
              << "  -k, --key <KEY>         API-ключ Google (или переменная YOUTUBE_API_KEY)\n"
              << "  -o, --output <FILE>     локальный файл для записи (по умолчанию /var/www/html/url.php)\n"
              << "  -s, --ssh <ARGS>        аргументы ssh, например \"user@host -p 12345\"\n"
              << "  -r, --remote-output <FILE>\n"
              << "                          файл на удалённом сервере (по умолчанию /var/www/html/url.php)\n"
              << "  -h, --help              показать эту справку\n";
}

Options parse_args(int argc, char** argv) {
    Options opts;
    if (const char* env_key = std::getenv("YOUTUBE_API_KEY")) {
        opts.api_key = env_key;
    }

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto value = [&]() -> std::string {
            if (i + 1 >= argc) {
                throw std::invalid_argument("для параметра " + arg + " не указано значение");
            }
            return argv[++i];
        };

        if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            std::exit(EXIT_SUCCESS);
        } else if (arg == "-k" || arg == "--key") {
            opts.api_key = value();
        } else if (arg == "-c" || arg == "--channel") {
            opts.channel_id = value();
        } else if (arg == "-o" || arg == "--output") {
            opts.output = value();
        } else if (arg == "-s" || arg == "--ssh") {
            opts.ssh = value();
        } else if (arg == "-r" || arg == "--remote-output") {
            opts.remote_output = value();
        } else {
            throw std::invalid_argument("неизвестный параметр: " + arg);
        }
    }

    if (opts.channel_id.empty()) {
        throw std::invalid_argument("не указан идентификатор канала (--channel)");
    }
    if (opts.api_key.empty()) {
        throw std::invalid_argument("не указан API-ключ (--key или YOUTUBE_API_KEY)");
    }
    return opts;
}

void write_local_file(const std::string& path, const std::string& video_id) {
    std::ofstream out(path, std::ios::trunc);
    if (!out) {
        throw std::runtime_error("не удалось открыть файл для записи: " + path);
    }
    out << "<script> jsvid = \"" << livestream::watch_url(video_id) << "\";</script>";
    if (!out) {
        throw std::runtime_error("ошибка записи в файл: " + path);
    }
}

// Дописывает идентификатор видео в удалённый файл через ssh
// и выводит то, что вернула удалённая сторона.
int append_remote(const Options& opts, const std::string& video_id) {
    const std::string command =
        "ssh " + opts.ssh + " echo " + video_id + " \\>\\> " + opts.remote_output;
    std::cout << command << '\n';

    FILE* pipe = popen(command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("не удалось запустить: " + command);
    }
    std::array<char, 256> line{};
    while (std::fgets(line.data(), static_cast<int>(line.size()), pipe)) {
        std::cout << line.data();
    }
    return pclose(pipe);
}

}  // namespace

int main(int argc, char** argv) {
    Options opts;
    try {
        opts = parse_args(argc, argv);
    } catch (const std::invalid_argument& e) {
        std::cerr << "Ошибка: " << e.what() << "\n\n";
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    try {
        livestream::CurlGlobal curl_global;

        const std::string response =
            livestream::http_get(livestream::build_search_url(opts.channel_id, opts.api_key));

        const auto video_id = livestream::extract_video_id(response);
        if (!video_id) {
            std::cerr << "Активная трансляция не найдена\n";
            return EXIT_FAILURE;
        }
        if (!livestream::is_valid_video_id(*video_id)) {
            std::cerr << "Некорректный идентификатор видео: " << *video_id << '\n';
            return EXIT_FAILURE;
        }

        std::cout << livestream::watch_url(*video_id) << '\n';
        write_local_file(opts.output, *video_id);

        if (!opts.ssh.empty()) {
            const int rc = append_remote(opts, *video_id);
            if (rc != 0) {
                std::cerr << "ssh завершился с кодом " << rc << '\n';
                return EXIT_FAILURE;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
