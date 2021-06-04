#include "file_handle.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>

#include "common/net_packet.h"
#include "common/base.h"

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
  		
