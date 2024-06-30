cmake_minimum_required(VERSION 3.27)

project(main)

message(STATUS "Project Name: ${PROJECT_NAME}")
message(STATUS "Project Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 将源代码添加到此项目的可执行文件。
aux_source_directory(. main_files)
add_executable(${PROJECT_NAME} ${main_files})

# 链接库头文件路径
target_include_directories(${PROJECT_NAME} PUBLIC ../image)

# 链接库文件夹
# 如果动态链接库和静态链接库同名，优先调用静态链接库
target_link_libraries(${PROJECT_NAME} PUBLIC image_static)

install(TARGETS ${PROJECT_NAME}
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION bin)

# # 链接库头文件路径
# target_include_directories(main
#        PUBLIC ${CMAKE_SOURCE_DIR}/FMT/include/fmt)

# # 链接库文件路径
# target_link_directories(main
#        PUBLIC ${CMAKE_SOURCE_DIR}/FMT/lib)

# # 链接库文件名称
# target_link_libraries(main
#        PUBLIC
#        debug fmtd
#        optimized fmt
# )