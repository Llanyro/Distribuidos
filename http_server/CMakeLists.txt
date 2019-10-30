cmake_minimum_required(VERSION 3.5)

project(http_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)
add_executable(http_server main.cpp httpserver.cpp httpserver.h utils.cpp utils.h)
target_link_libraries(${PROJECT_NAME} mysqlcppconn mysqlclient pthread z m rt atomic dl)
