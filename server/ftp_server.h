#pragma once

#include <map>
#include "iHandler.h"

#define LISTEN_QUEUE 128  /*maximum number of client connections*/

class FtpServer
{
	public:
		FtpServer();
		~FtpServer();
	
		void start(int port);
		
		void open_net(int port);
            	
		bool register_handler(int ops, IHandler *handler);
                void unregister_handler(int ops);

        protected:
        
	private:
                std::map<int, IHandler*> _handlers;     //消息处理器表
};
