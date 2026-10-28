cmake_minimum_required(VERSION 3.16)

project(get_livestream_url LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release)
endif()

find_package(CURL REQUIRED)

add_executable(get_livestream_url
    src/main.cpp
    src/http_client.cpp
    src/youtube.cpp
)

target_link_libraries(get_livestream_url PRIVATE CURL::libcurl)

if(MSVC)
    target_compile_options(get_livestream_url PRIVATE /W4 /utf-8)
else()
    target_compile_options(get_livestream_url PRIVATE -Wall -Wextra -Wpedantic)
endif()
