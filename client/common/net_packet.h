#pragma once
#include <string.h>

#include "error_code.h"
#include "ops.h"
#include "base.h"

#pragma pack(push, 1)	// 1字节对齐

#define BUFFER_SIZE 1024


//文件信息
typedef struct FileInfo
{
	int size;
	char type;
	char name[FILE_NAME_MAX];
}FileInfo;


//数据包
typedef struct NetPacket
{
	int ops;			//操作码
	int err;			//错误码
	bool finish;			//发送完毕					
	int length;			//包体长度
	char buff[BUFFER_SIZE];         //数据包缓冲区
	NetPacket()
	{
		init();
	}
	void init(void)
	{
		ops = 0;
		err = ERROR_BASE_SUCCESS; 
		finish = false;
		length = 0;
		memset(buff, 0, sizeof(buff)); 	
	}
}NetPacket;

#pragma pack(pop)
