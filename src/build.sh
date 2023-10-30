#!/bin/sh 

set -xe

CFALGS="-Wall -Wextra"

clang++ $CFLAGS -std=c++17 -I /usr/local/Cellar/asio-1.28.0/include simple-server.cpp -o simple-server

clang++ $CFLAGS -std=c++17 -I /usr/local/Cellar/asio-1.28.0/include simple-client.cpp -o simple-client