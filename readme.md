# get_livestream_url

Утилита на C++ для получения ссылки на текущую трансляцию (livestream) YouTube-канала.
Для работы необходимы API-ключ Google (YouTube Data API v3) и идентификатор канала.

Программа:

1. запрашивает `https://www.googleapis.com/youtube/v3/search` с параметром `eventType=live`;
2. извлекает `videoId` активной трансляции и выводит ссылку `https://www.youtube.com/watch?v=<id>`;
3. записывает в локальный файл строку `<script> jsvid = "https://www.youtube.com/watch?v=<id>";</script>`;
4. если указан параметр `--ssh`, дописывает идентификатор видео в файл на удалённом сервере
   командой `ssh <аргументы> echo <id> >> <файл>`.

## Зависимости

- компилятор с поддержкой C++17 (GCC, Clang, MSVC);
- CMake 3.16+;
- libcurl с заголовками (в Debian/Ubuntu — пакет `libcurl4-openssl-dev`).

## Сборка

```bash
cmake -S . -B build
cmake --build build
```

Исполняемый файл: `build/get_livestream_url`.

## Запуск

```bash
./build/get_livestream_url --channel UCxxxxxxxxxxxxxxxxxxxxxx --key AIza... \
    --output /var/www/html/url.php \
    --ssh "user@host -p 12345" --remote-output /var/www/html/url.php
```

API-ключ можно передать через переменную окружения `YOUTUBE_API_KEY` вместо `--key`.

| Параметр | Описание | По умолчанию |
|---|---|---|
| `-c`, `--channel` | идентификатор YouTube-канала | — (обязателен) |
| `-k`, `--key` | API-ключ Google | `$YOUTUBE_API_KEY` |
| `-o`, `--output` | локальный файл для записи | `/var/www/html/url.php` |
| `-s`, `--ssh` | аргументы ssh; без него удалённый шаг пропускается | — |
| `-r`, `--remote-output` | файл на удалённом сервере | `/var/www/html/url.php` |

Код возврата `0` — ссылка получена и записана; ненулевой — ошибка либо активная трансляция не найдена.
