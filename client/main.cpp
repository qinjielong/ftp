#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
#include "ftp_client.h"


int main(int argc, char **argv)
{
	if (argc !=3) {
  		std::cerr << "Usage: ./main <IP address of the server> <port number>" << std::endl;
  		return -1;
 	}
	
	std::cout << "List of Commands:" << std::endl;
	std::cout << "quit			exit process" << std::endl;
	std::cout << "ls			get server current file list" << std::endl;
	std::cout << "cd [path]			change server path" << std::endl;
	std::cout << "get [file]		download file from server" << std::endl;
	std::cout << "put [file]		upload file to server" << std::endl;
	
	FtpClient client(argv[1], atoi(argv[2]));
	
	std::cout << "ftp>";
	char buff[128] = {0};	
	while (fgets(buff, sizeof(buff), stdin) != NULL) {
		if (strcmp("quit\n", buff) == 0) {
   			break;
   		}
		else if (strcmp("ls\n", buff) == 0) {
			std::vector<std::string> vec;
			client.file_list(vec);
			std::vector<std::string>::iterator it = vec.begin();
	 		for(; it != vec.end(); ++it) {
	 			std::cout << (*it) << std::endl;
	 		}	
		}
		else {
			char *token = strtok(buff, " ");			
			if (strcmp("cd", token) == 0) {
				token = strtok(NULL," \n");
				client.change_dir(token);
			}
			else if (strcmp("get", token) == 0) {
				token = strtok(NULL," \n");
				client.download(token);	

			}
			else if (strcmp("put", token) == 0) {
				token = strtok(NULL," \n");
				client.upload(token);
			}
			else {
				std::cerr << "Error in command. Check Command" << std::endl;
			}
		}

		
		std::cout << "ftp>";
	}

	
	return 0;
}
