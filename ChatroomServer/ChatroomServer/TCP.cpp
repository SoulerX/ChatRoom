#include "TCP.h"

using namespace std;

SOCKET TCP::tcpSocketInit()
{
	// �汾���
	WORD require;
	require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)//�ɹ����ط���0��
	{
		cout << "loading winsock faild" << endl;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "request version faild" << endl;
	}
	cout << "request version successed" << endl;

	// ����SOCKET
	SOCKET severSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (severSocket == -1)
	{
		cout << "create socket faild" << endl;
	}
	cout << "create socket successed" << endl;

	// ����Э��
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = INADDR_ANY;
	addr.sin_addr.S_un.S_addr = inet_addr("192.168.111.1");
	//addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.87");
	addr.sin_port = htons(25000);

	int result; // ����ֵ

	// ��
	result = bind(severSocket, (sockaddr*)&addr, sizeof(addr));
	if (result == -1)
	{
		cout << "bind socket faild" << endl;
	}
	cout << "bind socket successed" << endl;

	// ����
	result = listen(severSocket, 10);
	if (result == -1)
	{
		cout << "listen faild" << endl;
	}
	cout << "listen successed" << endl;

	return severSocket;
}