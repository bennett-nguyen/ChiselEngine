cmake_minimum_required(VERSION 3.24)
project(Chisel)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    message(FATAL_ERROR "Unsupported platform.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)
find_package(GLEW REQUIRED)

set(INIT_SOURCE_FILES)
set(UTIL_SOURCE_FILES)
set(VOXEL_SOURCE_FILES)
set(MAIN_SOURCE_FILES src/main.cpp)

set(IMGUI_SOURCE_FILES)
set(STB_SOURCE_FILE deps/stb/stb.cpp)
set(DEPS_FILES)

file(GLOB INIT_SOURCE_FILES CONFIGURE_DEPENDS "src/init/*.cpp")
file(GLOB UTIL_SOURCE_FILES CONFIGURE_DEPENDS "src/util/*.cpp")
file(GLOB VOXEL_SOURCE_FILES CONFIGURE_DEPENDS "src/voxel/*.cpp")
file(GLOB IMGUI_SOURCE_FILES CONFIGURE_DEPENDS "deps/imgui/*.cpp")


set(SOURCE_FILES)

list (APPEND DEPS_FILES
    ${IMGUI_SOURCE_FILES}
    ${STB_SOURCE_FILE})
list (APPEND SOURCE_FILES
    ${INIT_SOURCE_FILES}
    ${UTIL_SOURCE_FILES}
    ${VOXEL_SOURCE_FILES}
    ${MAIN_SOURCE_FILES})

set_source_files_properties(
    ${SOURCE_FILES}
    PROPERTIES
    COMPILE_FLAGS "-O2 -DNDEBUG -Wall -Wextra -Wconversion -Wsign-conversion")

set_source_files_properties(
    ${DEPS_FILES} PROPERTIES COMPILE_FLAGS "-O2 -DNDEBUG"
)

include_directories(${PROJECT_NAME}
    include/
    include/imgui
    include/stb
    src/init
    src/util
    src/voxel
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${DEPS_FILES})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
    set(CMAKE_CXX_FLAGS "-static -static-libgcc -static-libstdc++")
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2-static GLEW::glew_s OpenGL::GL)
endif()

if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2 GLEW::GLEW OpenGL::GL)
endif()

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND bash -c "mkdir -pv ../bin/resources"
    VERBATIM
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND bash -c "cp -ruv ../src/resources ../bin"
    VERBATIM
)