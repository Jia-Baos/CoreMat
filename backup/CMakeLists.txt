cmake_minimum_required(VERSION 3.27)

project(main)

# 将源代码添加到此项目的可执行文件。
aux_source_directory(. main_files)
add_executable(main ${main_files})

set_target_properties(main
        PROPERTIES
        CXX_STANDARD 20
        CXX_EXTENSIONS OFF
        CXX_STANDARD_REQUIRED ON
)

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