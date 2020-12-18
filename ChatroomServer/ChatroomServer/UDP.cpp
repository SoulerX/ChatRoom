#include "UDP.h"

unsigned UDP::udpSocketInit()
{
	//执行WSAStarup函数
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//执行socket函数
	SOCKET serSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);			//使用UDP协议
	if (serSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}

	//规定地址的一些参数
	struct sockaddr_in serAddr;
	memset(&serAddr, 0, sizeof(sockaddr_in)); //每个字节都用0填充
	serAddr.sin_family = AF_INET; //使用IPv4地址
	serAddr.sin_port = htons(20000); //端口
	serAddr.sin_addr.S_un.S_addr = INADDR_ANY; //自动获取IP地址
	memset(&serAddr.sin_zero, 0, 8);
	
	//执行bind函数，将socket与地址Addr绑定
	if (bind(serSocket, (struct sockaddr*) & serAddr, sizeof(serAddr)) == SOCKET_ERROR)
	{
		printf("bind error !");
		closesocket(serSocket);
		return 0;
	}
	return serSocket;
}

int UDP::serverRecvMessage(PackInfo* pack, int serSocket, struct sockaddr_in* p_remoteAddr)
{
	int nAddrLen = sizeof(*p_remoteAddr);
	int ret = recvfrom(serSocket, (char*)pack, sizeof(PackInfo), 0, (struct sockaddr*)  p_remoteAddr, &nAddrLen);

	if (ret < 0)
	{
		return -1;
	}
	printf("recv %d bytes", ret);
	return ret;
}

int UDP::serverSendMessage(BackInfo* back, int serSocket, struct sockaddr_in* p_remoteAddr)
{
	int ret = sendto(serSocket, (char*)back, sizeof(back), 0, (const sockaddr*)p_remoteAddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		printf("send to error\n");
		return -1;
	}
	return ret;
}

int UDP::rserverRecvMessage(RBackInfo* back, int serSocket, struct sockaddr_in* p_remoteAddr)
{
	int nAddrLen = sizeof(*p_remoteAddr);
	int ret = recvfrom(serSocket, (char*)back, sizeof(RBackInfo), 0, (struct sockaddr*)  p_remoteAddr, &nAddrLen);

	if (ret < 0)
	{
		return -1;
	}
	printf("recv %d bytes", ret);
	return ret;
}

int UDP::rserverSendMessage(RPackInfo* pack, int serSocket, struct sockaddr_in* p_remoteAddr)
{
	int ret = sendto(serSocket, (char*)pack, sizeof(RPackInfo), 0, (const sockaddr*)p_remoteAddr, sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		printf("send to error\n");
		return -1;
	}
	return ret;
}