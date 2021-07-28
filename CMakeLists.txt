# CMake 最低版本号要求
cmake_minimum_required(VERSION 3.0)

# 项目名称
project(uart2tcp)

# 查找指定目录下的所有.cpp与.h文件 并存放到指定变量名SC_FILES中
FILE(GLOB SC_FILES "*.c" "*.h")

# 指定生成目标
#add_executable(${PROJECT_NAME} ${SC_FILES})
add_executable(uart2tcp ${SC_FILES})

target_link_libraries( uart2tcp -lwsock32)
