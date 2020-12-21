#include "TCP.h"

using namespace std;

SOCKET TCP::tcpSocketInit()
{
	WORD require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)
	{
		cout << "loading winsock faild" << endl;
	}
	cout << "loading winsock successed" << endl;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "request version faild" << endl;
	}
	cout << "loading version successed" << endl;

	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	if (sock == -1)
	{
		cout << "create socket faild" << endl;
	}
	cout << "create socket successed" << endl;

	return sock;
}

bool TCP::TcpConnect(SOCKET sock)
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	//addr.sin_addr.S_un.S_addr = inet_addr("192.168.111.1");
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.11");
	addr.sin_port = htons(25000);

	int b;
	b = connect(sock, (sockaddr*)&addr, sizeof(addr));
	if (b == -1)
	{
		cout << WSAGetLastError() << endl;
		cout << "connect faild" << endl;
		return false;
	}
	cout << "server has been successfully connected, you can send message" << endl;

	return true;
}

