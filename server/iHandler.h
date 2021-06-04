#pragma once
#include "common/net_packet.h"


class IHandler
{
	public:
		virtual ~IHandler(){};
		virtual void handle(const NetPacket &pack, int socket) = 0;
};
