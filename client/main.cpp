#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <iostream>
#include <vector>

#include "common/base.h"
#include "ftp_client.h"


int main(int argc, char **argv)
{
	if (argc !=3) {
  		std::cerr << "Usage: ./main <IP address of the server> <port number>" << std::endl;
  		return -1;
 	}
	
	std::cout << "List of Commands:" << std::endl;
	std::cout << "quit				exit process" << std::endl;
	std::cout << "ls				get server current file list" << std::endl;
	std::cout << "ls-c                      	get local current file list" << std::endl;
	std::cout << "cd-c [path]               	change local path" << std::endl;
	std::cout << "pwd-c     			get local current path" << std::endl;
	std::cout << "cd [path]			change server path" << std::endl;
	std::cout << "get [file]			download file from server" << std::endl;
	std::cout << "put [file]			upload file to server" << std::endl;
	
	char local_path[FILE_NAME_MAX] = {0};	
	if (NULL == getcwd(local_path, sizeof(local_path)-1)) {
		std::cout << "pwd error!!" <<  std::endl;
		return -1;
	}

	FtpClient client(argv[1], atoi(argv[2]));
	
	std::cout << "ftp>";
	char buff[FILE_NAME_MAX] = {0};	
	while (fgets(buff, sizeof(buff), stdin) != NULL) {
		if (strcmp("quit\n", buff) == 0) {
   			break;
   		}
		else if (strcmp("ls\n", buff) == 0) {
			std::vector<std::string> vec;
			client.file_list(vec);
			std::vector<std::string>::iterator it = vec.begin();
	 		for(; it != vec.end(); ++it) {
	 			std::cout << (*it);
	 		}	
		}
		else if (strcmp("ls-c\n", buff) == 0) {
 			FILE *in = popen("ls", "r");
  			char temp[128] = {0};
        		while(fgets(temp, sizeof(temp), in) != NULL){
                		std::cout << temp;
				memset(temp, 0, sizeof(temp));
			}

 			pclose(in);
			


		}
		else if (strcmp("pwd-c\n", buff) == 0){
 			char temp[128] = {0};
        		getcwd(temp, sizeof(temp)-1);
        		printf("%s\n", temp);
		}
		else {
			char *token = strtok(buff, " ");			
			if (strcmp("cd-c", token) == 0) {
				token = strtok(NULL," \n");
				if(chdir(token) < 0){
                			printf("no such directory:%s\n", token);
        			}
			}
			else if (strcmp("cd", token) == 0) {
				token = strtok(NULL," \n");
				client.change_dir(token);
			}
			else if (strcmp("get", token) == 0) {
				token = strtok(NULL," \n");
				client.download(local_path, token);	
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
