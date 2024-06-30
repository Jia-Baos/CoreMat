cmake_minimum_required(VERSION 3.27)

project(benchmark_image)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

aux_source_directory(. main_files)

add_executable(${PROJECT_NAME} ${main_files})

target_include_directories(${PROJECT_NAME} PUBLIC ../src/image)

target_link_libraries(${PROJECT_NAME} image_static benchmark::benchmark)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)
