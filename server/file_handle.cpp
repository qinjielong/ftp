#include "file_handle.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <fstream>
#include <vector>

#include "common/net_packet.h"
#include "common/base.h"


void* recv_msg_from_client(void* arg)  
{  
        // 分离线程，使主线程不必等待此线程  
        pthread_detach(pthread_self());  
          
        ThreadParam* param = (ThreadParam* )arg;  
      	int connfd = param->connfd;
	std::string path_server = param->path;
	
	NetPacket packet;
	while (true) {
		packet.init();
		int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
		if (count <= 0) {
			printf("recv error!\n");
			break;
		}
		
   		printf("packet received from client ops:%d\n", packet.ops);
		switch (packet.ops) {
		case OPS_FTP_FILE_LIST:
			on_file_list(connfd);
			break;				
		case OPS_FTP_FILE_UPLOAD:
			on_upload(path_server, packet.buff, connfd, packet.total);	
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
  		
int on_file_list(int connfd)
{
	NetPacket packet;
	FILE *in = popen("ls", "r");
	if(!in) {
		printf("open error\n");
		packet.err = ERROR_BASE_UNKNOWN; 
		send(connfd, (char*)&packet, sizeof(packet), 0);
		return -1;
	}
	
	char temp[FILE_NAME_MAX] = {0};
	std::vector<std::string> vec_line;
	while(fgets(temp, sizeof(temp), in) != NULL){
		std::cout << temp;	
		vec_line.push_back(temp);
	}
 	
	pclose(in);
	
	FileInfo file;
	size_t total = vec_line.size() * sizeof(file);
	size_t send_total = 0;
	auto it = vec_line.begin();
        for(; it != vec_line.end(); ++it) {
		memset(file.name, 0, sizeof(file.name));
		memcpy(file.name, (*it).c_str(), (*it).length());
			
                packet.init();
		packet.total = total;
		packet.length = sizeof(file);
		memcpy(packet.buff, (char*)&file, sizeof(file));

		int count = send(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0) {
			std::cout << "send error" << std::endl;
			break; 
		}
		
		send_total += packet.length;
		memset(temp, 0, sizeof(temp));
        }

  	if (send_total != total) {
                printf("error! need:%ld, send:%ld\n", total, send_total);
        }else{
                printf("file list done\n");
        }

	return 0;
}

int on_upload(const std::string &path_server, const char *path, int connfd, size_t total) 
{
	NetPacket packet;
	
	//上传文件暂存temp目录
	std::string temp = path;
	if (temp.length() <= 0) {
		packet.err = ERROR_BASE_PARAM; 
		send(connfd, (char*)&packet, sizeof(packet), 0);
		std::cout << "upload path error! " << path << std::endl;
		return -1;
	}
	
	if (temp[0] == '/' || temp[0] == '\\'){
		temp = path_server + "/temp" + temp; 
	}else{
		temp = path_server + "/temp/" + temp; 
	}
	
	if (0 != create_directory(temp)) {
		packet.err = ERROR_BASE_PARAM; 
		send(connfd, (char*)&packet, sizeof(packet), 0);
		std::cout << "create directory error! " << path << std::endl;
		return -1;
	}

	if (file_exists(temp.c_str())) {
		packet.err = ERROR_BASE_FILE_EXIST; 
	}

	// 通知客户端可以传输文件了
	int count = send(connfd, (char*)&packet, sizeof(packet), 0);
	if (count <= 0) {
		printf("on_upload send msg fail\n");
		return -1;
	}
	
	std::cout << "on_upload: opening the file " << temp  << " for writing" << std::endl;
	std::ofstream os(temp.c_str(), std::ios::out | std::ios::binary);
	if (!os) {
		std::cout << "on_upload: can not open!" << std::endl; 
		return -1;
	}

	size_t recv_total = 0;
        while (true) {
                packet.init();
                int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0) {
                        printf("recv error!\n");
                        break;
                }
		
		os.write(packet.buff, packet.length);	
		recv_total += packet.length;
		if (total == recv_total) {
			break;
		}
	}	
	
	os.close();
 	if (recv_total != total) {
                printf("error! need:%ld, recv:%ld\n", total, recv_total);
        }else{
                printf("file upload done\n");
        }

	return 0; 
}

int on_download(const char *path, int connfd) 
{
	size_t total = get_file_size(path);
	NetPacket packet;
	packet.total = total;
	if (!file_exists(path)) {
		packet.err = ERROR_BASE_NO_FILE; 
	}
	
	// 通知客户端准备接收文件了
	int count = send(connfd, (char*)&packet, sizeof(packet), 0);
	if (count <= 0) {
		printf("on_download send msg fail\n");
		return -1;
	}

	std::cout << "on_download: opening the file " << path << " for reading" << std::endl;
	std::ifstream is(path, std::ios::in | std::ios::binary);
	if (!is) {
		std::cout << "on_download: can not open!" << std::endl; 
		return -1;
	}

        size_t send_total = 0;
	while (!is.eof()) {
                packet.init();
        	is.read(packet.buff, BUFFER_SIZE);       
		packet.length = is.gcount();		
	 	
		int count = send(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0) {
			std::cout << "send error" << std::endl;
			break; 
		}
		
		send_total += packet.length;
	}
	
	is.close();
	if (send_total != total) {
                printf("error! need:%ld, send:%ld\n", total, send_total);
        }else{
                printf("file download done\n");
        }
	
	return 0;
}

