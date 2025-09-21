cmake_minimum_required(VERSION 4.0)

add_library(updater_lib
        src/Updater.cpp
)

target_compile_features(updater_lib PRIVATE cxx_std_20)

target_include_directories(updater_lib
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# to this:
add_executable(updater WIN32
        src/main_updater.cpp
)

if (MINGW)
    # belt-and-suspenders for MinGW
    target_link_options(updater PRIVATE -mwindows)
endif()

# (optional) keep a console in Debug builds only:
if (MINGW)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_link_options(updater PRIVATE -mconsole)
    endif()
endif()

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

# … your existing targets …

if (WIN32)
    # Embed version info + manifest (works with MinGW windres or MSVC’s rc.exe)
    target_sources(updater PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/version.rc)
endif()

