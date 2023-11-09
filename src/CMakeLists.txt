cmake_minimum_required(VERSION 3.24.4)

project(HTTP-SOCK)

add_executable(${PROJECT_NAME} simple-client.cpp)
install(TARGETS HTTP-SOCK DESTINATION usr/local/Cellar)
target_link_libraries(-L/usr/local/Cellar/asio-1.28.0/include)