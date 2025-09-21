cmake_minimum_required(VERSION 4.0)

set(GAME_TARGET BeneathTheSurface CACHE STRING "Beneath The Surface")

add_executable(${GAME_TARGET}
        src/Game.cpp
        ../updater/src/Updater.cpp
        src/update/GameStartupUpdateCheck.cpp
        include/update/GameStartupUpdateCheck.h
        include/graphics/Window.h
        include/Game.h
        src/graphics/Window.cpp
        include/util/Input.h
)

target_compile_features(${GAME_TARGET} PRIVATE cxx_std_20)

target_include_directories(${GAME_TARGET}
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(${GAME_TARGET}
        PRIVATE
        updater_lib
        OpenGL::GL
        GLEW::GLEW
        glfw
        glm::glm
)

set(GAME_RES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/resources)
if(EXISTS ${GAME_RES_DIR})
    add_custom_command(TARGET ${GAME_TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${GAME_RES_DIR}
            $<TARGET_FILE_DIR:${GAME_TARGET}>/resources
            COMMENT "Copying game resources"
    )
    file(GLOB_RECURSE GAME_RES_FILES "${GAME_RES_DIR}/*")
    add_custom_target(${GAME_TARGET}_resources SOURCES ${GAME_RES_FILES})
endif()

if(TARGET updater)
    target_link_libraries(${GAME_TARGET} PRIVATE updater)
    target_compile_definitions(${GAME_TARGET} PRIVATE HAS_INTERNAL_UPDATER=1)
endif()
