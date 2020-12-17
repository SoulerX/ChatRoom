#include "stdafx.h"

class TCP
{
	WSADATA wsaData;

public:
	SOCKET tcpSocketInit(); // TCP初始化
	bool TcpConnect(SOCKET sock); // 建立连接

};