#include "ftp_client.h"


int main()
{
	FtpClient client("0.0.0.0", 9001);
	client.change_dir("..");
	
	std::vector<std::string> vec;
	client.file_list(vec);
	//client.upload("client.cpp");
	//client.download("Makefile");	
	return 0;
}
