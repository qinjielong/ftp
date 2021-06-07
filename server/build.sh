#!/bin/bash
g++ -o main common/base.cpp main.cpp file_handle.cpp ftp_server.cpp -std=c++11 -DNDEBUG -Wall -lpthread
