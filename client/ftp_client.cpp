#include "ftp_client.h"
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <fstream>


#include "common/net_packet.h"


FtpClient::FtpClient(const char *addr, int port){
	memset(&_server_addr, 0, sizeof(_server_addr));
		
	_server_addr.sin_family = AF_INET;           
	if (inet_pton(AF_INET, addr, &_server_addr.sin_addr) <= 0) {
        	printf("inet_pton error for %s\n", addr);
    	}

	_server_addr.sin_port = htons(port); 
}

FtpClient::~FtpClient(){

}

void FtpClient::file_list(std::vector<std::string> &names) {
	int connfd = create_socket();
	NetPacket packet;
	packet.ops = 2;
	
	send(connfd, (char*)&packet, sizeof(packet), 0);
	
        while (true) {
                packet.init();
                int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0) {
                        printf("recv error!\n");
                        break;
                }
	
		if (packet.finish) {
			printf("recv finish!!\n");
			break;
		}
		FileInfo *f = (FileInfo *)packet.buff;
		names.push_back(f->name);
	}
	close(connfd);			
}

bool FtpClient::upload(const char *path) {
	int connfd = create_socket();
	
	// 通知服务器开始上传文件
	NetPacket p;
	p.ops = 3;
	std::string name = get_file_name(path);
	memcpy(p.buff, name.c_str(), name.length());
	send(connfd, (char*)&p, sizeof(p), 0);
	recv(connfd, (char*)&p, sizeof(p), 0);
	if (p.err != 0){
		std::cout << "put err:" << p.err << std::endl;
	}
	
	
	// 读取文件
	std::cout << "opening file" << path << std::endl;;
	std::ifstream is(path, std::ios::in | std::ios::binary);
	if (!is){
		std::cout << "Error opening the file to read" << std::endl;
		close(connfd);	
		return false;		
	}
	
	while (is) {
		p.init();
		
		is.read(p.buff, BUFFER_SIZE);
		p.length = is.gcount();		
		int count = send(connfd, (char*)&p, sizeof(p), 0);
		if (count <= 0) {
			std::cout << "upload send msg error" << std::endl;
			break;
		}
	}

	is.close();
	
	// 文件发送完成
	p.init();
	p.finish = true;
	int count = send(connfd, (char*)&p, sizeof(p), 0);
	if (count <= 0) {
		std::cout << "upload send finish msg error" << std::endl;
		return false;
	}

	close(connfd);			
	std::cout<<"File upload done" << std::endl;
	return true;
}

bool FtpClient::download(const char *local_path, const char *path) {
	int connfd = create_socket();
	
	// 告诉服务器开始下载文件
	NetPacket packet;
	packet.ops = 4;
	memcpy(packet.buff, path, strlen(path));
	send(connfd, (char*)&packet, sizeof(packet), 0);
	recv(connfd, (char*)&packet, sizeof(packet), 0);	
	if (packet.err != 0){
		std::cout << "put err:" << packet.err << std::endl;
		close(connfd);			
		return false;
	}

	// 检查文件目录
	std::string temp = local_path;
	temp.append("/temp/");
	
	std::string name = get_file_name(path);
	printf("path:%s, name:%s", path, name.c_str());
	temp.append(name);
        if (0 != create_directory(temp)) {
                std::cout << "create directory error! " << temp << std::endl;
		close(connfd);			
                return false;
        }

        if (file_exists(temp.c_str())) {
                std::cout << "file exist, should override it!" << std::endl;
        }
	
	// 打开文件并写入
	std::cout << "download: opening the file " << temp << " for writing" << std::endl;
	std::ofstream os(temp.c_str(), std::ios::out | std::ios::binary);
	if (!os) {
		std::cout << "download: can not open!" << std::endl; 
		close(connfd);			
		return false;
	}

        while (true) {
                packet.init();
                int count = recv(connfd, (char *)&packet, sizeof(packet), 0);
                if (count <= 0) {
                        printf("recv error!\n");
                        break;
                }
		
		if (packet.finish && 0 == packet.length) {
			printf("recv finish!!\n");
			break;
		}
		os.write(packet.buff, packet.length);	
	}
	
	os.close();
	close(connfd);			
	std::cout << "download done" << std::endl;
	return true;
}

bool FtpClient::change_dir(const char *path) {
	int connfd = create_socket();
	NetPacket packet;
	packet.ops = 5;
	memcpy(packet.buff, path, strlen(path));
	send(connfd, (char*)&packet, sizeof(packet), 0);
	close(connfd);			
	return true;
}

int FtpClient::create_socket() {
	int sock = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
	if(sock < 0) {
		printf("socket error\n");
		return -1;
	}

	setsockopt(sock, SOL_SOCKET,SO_REUSEADDR, (char *)(1), sizeof (1));
	if (connect(sock, (struct sockaddr *) &_server_addr, sizeof(_server_addr)) < 0) {
		printf("can not connect!!\n");
		return -1;		
	}	
	return sock;	
}
