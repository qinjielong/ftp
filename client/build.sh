#!/bin/bash

g++ -c common/base.cpp ftp_client.cpp -fPIC
 
g++ -shared -o libftp.so base.o ftp_client.o

g++ -g -o main main.cpp -L ./ -lftp

rm ./*.o -rf

# temp
export LD_LIBRARY_PATH=./
