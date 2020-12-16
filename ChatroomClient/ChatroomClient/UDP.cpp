#include "UDP.h"

int UDP::clientSocketInit()
{
	//ִ��WSAStarup����
	WSADATA wsaData;
	WORD sockVersion = MAKEWORD(2, 2);
	if (WSAStartup(sockVersion, &wsaData) != 0)
	{
		return 0;
	}

	//ִ��socket����
	SOCKET cliSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);			//ʹ��UDPЭ��
	if (cliSocket == INVALID_SOCKET)
	{
		printf("socket error !");
		return 0;
	}
	return cliSocket;
}

int UDP::clientSendMessage(PackInfo* pack, int cliSocket, struct sockaddr_in* p_serAddr)
{
	//sock�����ڴ��� UDP ���ݵ��׽��֣�
	//buf��������������ݵĻ�������ַ��
	//nbytes�����������ݵĳ��ȣ����ֽڼƣ���
	//flags����ѡ���������û�пɴ��� 0��
	//to������Ŀ���ַ��Ϣ�� sockaddr �ṹ������ĵ�ַ��
	//addrlen�����ݸ����� to �ĵ�ֵַ�ṹ������ĳ��ȡ�
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