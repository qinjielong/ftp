#include "ftp_server.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

#include "file_handle.h"

FtpServer::FtpServer()
{
 

}

FtpServer::~FtpServer()
{
 

}

bool FtpServer::register_handler(int ops, IHandler *handler)
{
	auto iter = _handlers.find(ops);
	if (iter != _handlers.end())
	{
		printf("already register ops:%d\n", ops);
		return false;
	}
	
	_handlers.insert(std::make_pair(ops, handler));
	return true;
}

void FtpServer::unregister_handler(int ops)
{
	auto iter = _handlers.find(ops);
	if (iter != _handlers.end())
	{
		_handlers.erase(iter);
	}
}

void FtpServer::start(int port)
{
 	// start worker

	open_net(port);
}

void FtpServer::open_net(int port)
{
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(struct sockaddr_in));

	// Create a socket for the soclet
 	//If sockfd<0 there was an error in the creation of the socket
 	
	int listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0) 
	{
  		printf("Problem in creating the socket\n");
  		exit(2);
 	}	
	
	int optval = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

 	servaddr.sin_family = AF_INET;
 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

 	bind (listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

 	//listen to the socket by creating a connection queue, then wait for clients
 	listen (listenfd, LISTEN_QUEUE); 

	struct sockaddr_in client_addr;
  	socklen_t addr_len = sizeof(client_addr);

	for ( ; ; ) 
	{
  		//accept a connection
  		int connfd = accept (listenfd, (struct sockaddr *) &client_addr, &addr_len);
		if (connfd < 0)  
    		{  
        		printf("error comes when call accept!\n");  
        		break;  
    		}  
		
		// 创建线程处理		
		pthread_t recv_id ; 
		pthread_create(&recv_id, NULL, recv_msg_from_client, &connfd);  
	}
}
