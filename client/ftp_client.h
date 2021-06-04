#pragma once
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <vector>

class FtpClient
{
	public:
		FtpClient(const char *addr, int port);
		~FtpClient();

		void file_list(std::vector<std::string> &names);
		bool download(const char *path);
		bool upload(const char *path);
		bool change_dir(const char *path);
		
	protected:
		int create_socket();

	private:
		struct sockaddr_in _server_addr;	
};
