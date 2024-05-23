cmake_minimum_required(VERSION 3.27)

project(image VERSION 1.0.0)

# message(STATUS "3rd name: ${PROJECT_NAME}")
# message(STATUS "3rd source dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 定义项目版本号宏
set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

# 生成版本号字符串
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project version: ${PROJECT_VERSION}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory(. main_files)

add_library(image_static STATIC ${main_files})
add_library(image_shared SHARED ${main_files})

set_target_properties(image_static PROPERTIES OUTPUT_NAME "image.${PROJECT_VERSION}")
set_target_properties(image_shared PROPERTIES OUTPUT_NAME "image.${PROJECT_VERSION}")

target_include_directories(image_static PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(image_shared PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

install(TARGETS image_static image_shared
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        DESTINATION ${CMAKE_INSTALL_PREFIX}/include
        FILES_MATCHING
        PATTERN "*.h"
        PATTERN "*.hpp"
        PATTERN "CMakeLists.txt" EXCLUDE)