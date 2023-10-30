cmake_minimum_required(VERSION 3.27.7)

project(HTTP-SOCK)

add_executable(${PROJECT_NAME} simple-client.cpp)
install(TARGETS HTTP-SOCK DESTINATION usr/local/Cellar)