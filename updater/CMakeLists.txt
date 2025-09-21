cmake_minimum_required(VERSION 4.0)

add_library(updater_lib
        src/Updater.cpp
)

target_compile_features(updater_lib PRIVATE cxx_std_20)

target_include_directories(updater_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(updater
        src/main_updater.cpp
)

target_compile_features(updater PRIVATE cxx_std_20)

find_package(CURL REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(LibArchive REQUIRED)

target_link_libraries(updater_lib
        PRIVATE CURL::libcurl nlohmann_json::nlohmann_json LibArchive::LibArchive
)
target_link_libraries(updater PRIVATE updater_lib)

target_include_directories(updater PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)
