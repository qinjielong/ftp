# An FTP client and server using the TCP protocol 

# Envoirment
- OS:CentOS 7.9
- Complier编译器 g++ 4.8

# Server quick start:

## Build 
./build.sh
or
make

## Usage
./main [port number]

## 上传的文件会放在运行目录的temp目录下


# Client quick start:

## Build
./build.sh

## Usage
./main [IP address of the server] [port number]

## List of Commands:
### quit ------------------- exit process
### ls-c	------------------- get local current file list
### cd-c [path]	------------ change local path
### pwd-c ------------------ get local current path
### ls --------------------- get server current file list
### cd [path] -------------- change server path
### get [file] ------------- download file from server
### put [file] ------------- upload file to server

## Example
### get /user/local/test.txt
### get test.txt

### put /user/local/test.txt
### put test.txt

## 下载的文件会放在运行目录的temp目录下
