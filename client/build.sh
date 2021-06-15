#!/bin/bash

g++ -c common/base.cpp ftp_client.cpp -fPIC -O3 -Wall
 
g++ -shared -o libftp.so base.o ftp_client.o -Wall

g++ -g -o main main.cpp -L ./ -lftp -O3 -Wall

rm ./*.o -rf

# temp
export LD_LIBRARY_PATH=./
