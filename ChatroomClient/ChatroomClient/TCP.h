#include "stdafx.h"

class TCP
{
	WSADATA wsaData;

public:
	SOCKET tcpSocketInit(); // TCP��ʼ��
	bool TcpConnect(SOCKET sock); // ��������

};