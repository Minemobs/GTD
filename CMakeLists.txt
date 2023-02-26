cmake_minimum_required(VERSION 3.21)
project(gradleTemplateDownloader)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git
        GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(fmt GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 8f8a1a02d5c5cb967d240feee3ffac00d66f22a2)
FetchContent_MakeAvailable(fmt)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)
FetchContent_MakeAvailable(json)

add_executable(gradleTemplateDownloader main.cpp)
target_link_libraries(gradleTemplateDownloader PRIVATE nlohmann_json::nlohmann_json cpr::cpr)
set_target_properties(gradleTemplateDownloader PROPERTIES LINKER_LANGUAGE CXX)