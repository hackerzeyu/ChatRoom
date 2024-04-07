cmake_minimum_required(VERSION 3.16)
project(chatserver)

set(CMAKE_CXX_STANDARD 17)

# 添加头文件路径
include_directories(
        ${PROJECT_SOURCE_DIR}/Log
        ${PROJECT_SOURCE_DIR}/Server
        ${PROJECT_SOURCE_DIR}/utility
        ${PROJECT_SOURCE_DIR}/thread
        ${PROJECT_SOURCE_DIR}/Handle
        ${PROJECT_SOURCE_DIR}/database
)

file(GLOB_RECURSE SRC "utility/*.cpp" "Server/*.cpp" "Log/*.cpp" "thread/*.cpp" "Handle/*.cpp" "database/*.cpp")

add_executable(chatserver main.cpp ${SRC})
target_link_libraries(chatserver pthread event jsoncpp mysqlclient)