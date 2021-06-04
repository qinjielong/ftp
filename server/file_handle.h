#pragma once

void* recv_msg_from_client(void* arg);  

int on_download(const char *name, int connfd);
int on_upload(const char *name, int connfd); 
int on_file_list(int connfd);
