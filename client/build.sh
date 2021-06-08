#!/bin/bash

g++ -c common/base.cpp ftp_client.cpp -fPIC -O3
 
g++ -shared -o libftp.so base.o ftp_client.o

g++ -g -o main main.cpp -L ./ -lftp -O3

rm ./*.o -rf

# temp
export LD_LIBRARY_PATH=./
