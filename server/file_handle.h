#pragma once
#include <unistd.h>
#include <string>
#include "common/net_packet.h"

typedef struct ThreadParam
{
	char connfd;
	char path[FILE_NAME_MAX];
}ThreadParam;

void* recv_msg_from_client(void* arg);  

int on_file_list(int connfd);

int on_upload(const std::string &path_server, const char *path, int connfd, size_t total); 

int on_download(const char *name, int connfd);
