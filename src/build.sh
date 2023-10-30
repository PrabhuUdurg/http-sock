#!/bin/sh 

set -xe

CFALGS="-Wall -Wextra"

clang++ $CFLAGS -std=c++17 -I /home/prabhu/Documents/Projects/asio-1.28.0/include simple-server.cpp -o simple-server -lX11

clang++ $CFLAGS -std=c++17 -I /home/prabhu/Documents/Projects/asio-1.28.0/include simple-client.cpp -o simple-client -lX11