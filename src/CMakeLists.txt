# 定义源文件列表
set(SOURCES
    hal/sgl_hal.c
    draw/sgl_draw.c
)

# 创建静态库 simplegl
add_library(simplegl STATIC ${SOURCES})

# 设置头文件包含路径
# PUBLIC 意味着链接这个库的目标也能看到这些头文件路径
target_include_directories(simplegl PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})