#include "ftp_server.h"

int main (int argc, char **argv)
{
	if (argc !=2) {	//validating the input
  		printf("Usage: ./a.out <port number>\n");
  		return 1;
 	}

	FtpServer* server = new FtpServer();	
	server->start(atoi(argv[1]));
	return 0;
}
