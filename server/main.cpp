#include "ftp_server.h"
#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	if (argc != 2) {
  		printf("Usage: ./main <port number>\n");
  		return 1;
 	}

	FtpServer* server = new FtpServer();	
	server->start(atoi(argv[1]));
	return 0;
}
