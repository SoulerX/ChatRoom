#include "UDP.h"

int UDP::clientSocketInit()
{
	//执行WSAStarup函数
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//执行socket函数
	SOCKET cliSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);			//使用UDP协议
	if (cliSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	return cliSocket;
}

int UDP::clientSendMessage(PackInfo* pack, int cliSocket, struct sockaddr_in* p_serAddr)
{
	//sock：用于传输 UDP 数据的套接字；
	//buf：保存待传输数据的缓冲区地址；
	//nbytes：带传输数据的长度（以字节计）；
	//flags：可选项参数，若没有可传递 0；
	//to：存有目标地址信息的 sockaddr 结构体变量的地址；
	//addrlen：传递给参数 to 的地址值结构体变量的长度。
	int ret = sendto(cliSocket, (char*)pack, sizeof(PackInfo), 0, (const sockaddr*)p_serAddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		return -1;
	}
	printf("send %d bytes", ret);
	return ret;
}

int UDP::clientRecvMessage(BackInfo* back, int cliSocket)
{
	int ret = recvfrom(cliSocket, (char*)back, sizeof(BackInfo), 0, NULL, NULL);
	if (ret < 0)
	{
		return -1;
	}
	return ret;
}

int UDP::rclientSendMessage(RBackInfo* back, int cliSocket, struct sockaddr_in* p_serAddr)
{
	int ret = sendto(cliSocket, (char*)back, sizeof(RBackInfo), 0, (const sockaddr*)p_serAddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		return -1;
	}
	printf("send %d bytes", ret);
	return ret;
}

int UDP::rclientRecvMessage(RPackInfo* pack, int cliSocket)
{
	int ret = recvfrom(cliSocket, (char*)pack, sizeof(RPackInfo), 0, NULL, NULL);
	if (ret < 0)
	{
		return -1;
	}
	return ret;
}