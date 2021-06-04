#!/bin/bash

g++ -c ftp_client.cpp -fPIC -o ftp_client.o
 
g++ -shared -o libftp.so ftp_client.o

g++ -o test test.cpp -L ./ -lftp
