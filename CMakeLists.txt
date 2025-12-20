cmake_minimum_required(VERSION 3.10)

project(simplegl C)

# 设置 C 标准
set(CMAKE_C_STANDARD 99)

# 添加子目录
add_subdirectory(src)
add_subdirectory(screen_render_demo)
add_subdirectory(font_render_demo)