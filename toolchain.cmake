set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)

# 修改这里的路径为你实际的 SDK 路径
set(tools /opt/aarch64-buildroot-linux-gnu_sdk-buildroot)
set(CMAKE_C_COMPILER ${tools}/bin/aarch64-linux-gcc)
set(CMAKE_CXX_COMPILER ${tools}/bin/aarch64-linux-g++)

set(CMAKE_SYSROOT ${tools}/aarch64-buildroot-linux-gnu/sysroot)

set(ENV{PKG_CONFIG_DIR} "")
set(ENV{PKG_CONFIG_LIBDIR} "${CMAKE_SYSROOT}/usr/lib/pkgconfig:${CMAKE_SYSROOT}/usr/share/pkgconfig")
set(ENV{PKG_CONFIG_SYSROOT_DIR} "${CMAKE_SYSROOT}")

# 搜索程序的行为：只在宿主机搜索
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# 搜索库和头文件的行为：只在目标机环境搜索
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)