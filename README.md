An FTP client and server using the TCP protocol 

server quick start:

./build.sh
or
make

./main <port number>


client quick start:

./build.sh
./main <IP address of the server> <port number>

 List of Commands:
 quit                      exit process" << std::endl;
 ls                        get server current file list" << std::endl;
 cd [path]                 change server path" << std::endl;
 get [file]                download file from server" << std::endl;
 put [file]                upload file to server" << std::endl;
