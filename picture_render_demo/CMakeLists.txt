# 创建可执行文件
add_executable(picture_render_demo main.c)

# 链接 simplegl 库
# 注意：simplegl 库是在 src/CMakeLists.txt 中定义的，这里直接引用即可
target_link_libraries(picture_render_demo PRIVATE simplegl m)