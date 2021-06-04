#include "ftp_server.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

#include "common/net_packet.h"
#include "common/base.h"

void* recv_msg_from_client(void* arg);
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

int on_file_list(int connfd)
{
	NetPacket packet;
	FILE *in = popen("ls", "r");
	if(!in)
	{
		printf("open error\n");
		packet.err = ERROR_BASE_UNKNOWN; 
		send(connfd, (char*)&packet, sizeof(packet), 0);
		return -1;
	}
	
	char temp[FILE_NAME_MAX] = {0};
	while(fgets(temp, sizeof(temp), in)!=NULL){
		printf("ls:%s\n", temp);	
		
		FileInfo f;
		memcpy(f.name, temp, sizeof(temp));
			
                packet.init();
		memcpy(packet.buff, (char*)&f, sizeof(f));
 
		int count = send(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0)
		{
			std::cout << "send error" << std::endl;
			break; 
		}
	}
	pclose(in);

	packet.finish = true;
	send(connfd, (char *)&packet, sizeof(packet), 0);
	std::cout << "list finish!" << std::endl;
	return 0;
}

int on_upload(const char *name, int connfd) 
{
	NetPacket packet;
	if (file_exists(name)) {
		packet.err = ERROR_BASE_FILE_EXIST; 
	}
	
	send(connfd, (char*)&packet, sizeof(packet), 0);
	
	std::cout << "on_upload: opening the file " << name << " for writing" << std::endl;
	std::ofstream os(name, std::ios::out | std::ios::binary);
	if (!os)  
	{
		std::cout << "on_upload: can not open!" << std::endl; 
		return 0;
	}

        while (true)
        {
                packet.init();
                int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0)
                {
                        printf("recv error!\n");
                        break;
                }
		
		os.write(packet.buff, packet.length);	
		if (packet.finish)
		{
			break;
		}
	}	
	os.close();
	std::cout << "file recv finish!" << std::endl;
	return 0; 
}

int on_download(const char *name, int connfd) 
{
	NetPacket packet;
	if (!file_exists(name)) {
		packet.err = ERROR_BASE_NO_FILE; 
	}
	
	send(connfd, (char*)&packet, sizeof(packet), 0);
	
	std::cout << "on_download: opening the file " << name << " for reading" << std::endl;
	std::ifstream is (name, std::ios::in | std::ios::binary);
	if (!is)  
	{
		std::cout << "on_download: can not open!" << std::endl; 
		return 0;
	}

        while (is)
        {
                packet.init();
        	is.read(packet.buff, BUFFER_SIZE);       
		packet.length = is.gcount();		
		if (packet.length < BUFFER_SIZE)
			packet.finish = true; 
	 	
		int count = send(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0)
		{
			std::cout << "send error" << std::endl;
			break; 
		}
	}
	is.close();
	std::cout << "file send finish!" << std::endl; 
	return 0;
}

void* recv_msg_from_client(void* arg)  
{  
        // 分离线程，使主线程不必等待此线程  
        pthread_detach(pthread_self());  
          
        int connfd = *(int*)arg;  
      
	NetPacket packet;
	while (true)
	{
		packet.init();
		int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
		if (count <= 0) 
		{
			printf("recv error!\n");
			break;
		}
		
   		printf("packet received from client ops:%d\n", packet.ops);
		switch (packet.ops) {
		case OPS_FTP_FILE_LIST:
			on_file_list(connfd);
			break;				
		case OPS_FTP_FILE_UPLOAD:
			on_upload(packet.buff, connfd);	
			break;			
		case OPS_FTP_FILE_DOWNLOAD:
			on_download(packet.buff, connfd);
			break;				
		case OPS_FTP_CHANGE_DIR:
			change_dir(packet.buff);	
			break;				
		}	
      	}

	printf("socket close!!\n");
        
	close(connfd);  
        return NULL;  
}
  		
