cmake_minimum_required(VERSION 3.21)
project(gradleTemplateDownloader)

set(CMAKE_CXX_STANDARD 20)

add_executable(gradleTemplateDownloader main.cpp)

if(WIN32) # Install dlls in the same directory as the executable on Windows
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
endif()


#========== Add CPM package manager =================
set(CPM_DOWNLOAD_VERSION 0.35.0)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}/cmake/CPM_${CPM_DOWNLOAD_VERSION}.cmake")

if(NOT (EXISTS ${CPM_DOWNLOAD_LOCATION}))
    message(STATUS "Downloading CPM.cmake")
    file(DOWNLOAD https://github.com/TheLartians/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake ${CPM_DOWNLOAD_LOCATION})
endif()

include(${CPM_DOWNLOAD_LOCATION})

#========== Add dependency =================

CPMAddPackage("gh:libcpr/cpr#1.8.1")
CPMAddPackage(
        NAME nlohmann_json
        VERSION 3.10.5
        GITHUB_REPOSITORY nlohmann/json
        OPTIONS
        "JSON_BuildTests OFF"
)
target_link_libraries(gradleTemplateDownloader PRIVATE nlohmann_json::nlohmann_json cpr::cpr)
