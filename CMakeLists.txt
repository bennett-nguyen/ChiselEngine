cmake_minimum_required(VERSION 3.24)
project(Chisel CXX C)

if (NOT (${CMAKE_SYSTEM_NAME} STREQUAL "Windows" OR ${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    message(FATAL_ERROR "Unsupported platform.")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin)

find_package(OpenGL REQUIRED)
find_package(SDL2 REQUIRED)

set(INIT_SOURCE_FILES)
set(UTIL_SOURCE_FILES)
set(VOXEL_SOURCE_FILES)
set(MAIN_SOURCE_FILES src/main.cpp)

set(IMGUI_SOURCE_FILES)
set(STB_SOURCE_FILE deps/stb/stb.cpp)
set(GLAD_SOURCE_FILE deps/glad.c)
set(DEPS_FILES)

file(GLOB INIT_SOURCE_FILES CONFIGURE_DEPENDS "src/init/*.cpp")
file(GLOB UTIL_SOURCE_FILES CONFIGURE_DEPENDS "src/util/*.cpp")
file(GLOB VOXEL_SOURCE_FILES CONFIGURE_DEPENDS "src/voxel/*.cpp")
file(GLOB IMGUI_SOURCE_FILES CONFIGURE_DEPENDS "deps/imgui/*.cpp")

list (APPEND DEPS_FILES
    ${IMGUI_SOURCE_FILES}
    ${STB_SOURCE_FILE}
)

list (APPEND SOURCE_FILES
    ${INIT_SOURCE_FILES}
    ${UTIL_SOURCE_FILES}
    ${VOXEL_SOURCE_FILES}
    ${MAIN_SOURCE_FILES})

include_directories(${PROJECT_NAME}
    include/
    include/glad
    include/imgui
    include/stb
    src/init
    src/util
    src/voxel
)

set(WARNING_FLAGS "-Wall -Wextra -Wconversion -Wsign-conversion")
set(CMAKE_CXX_FLAGS_DEBUG "-g3 ${WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

# Suppress all warnings, debugging information, and perform optimizations for dependency files
set_source_files_properties(${DEPS_FILES} PROPERTIES COMPILE_FLAGS "-w -g0 -O2 -DNDEBUG")

# GLAD Source file flags will be default to the build type configuration
set_source_files_properties(${GLAD_SOURCE_FILE} PROPERTIES COMPILE_FLAGS "")

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${GLAD_SOURCE_FILE} ${DEPS_FILES})

if (${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
    add_definitions(-DGLM_ENABLE_EXPERIMENTAL)
    add_compile_options(-static -static-libgcc -static-libstdc++)
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2-static OpenGL::GL)
endif()

if ((${CMAKE_SYSTEM_NAME} STREQUAL "Linux"))
    target_link_libraries(${PROJECT_NAME} SDL2::SDL2 OpenGL::GL)
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