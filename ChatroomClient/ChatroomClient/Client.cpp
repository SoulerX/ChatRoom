#include "Client.h"


Client::Client(CommunicationType type) // 默认TCP初始化
{
	WORD require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)
	{
		cout << "loading winsock faild" << endl;
		return;
	}
	cout << "loading winsock successed" << endl;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "request version faild" << endl;
		return;
	}
	cout << "loading version successed" << endl;

	if (Init(type))
	{
		cout << "init finish\n\n\n" << endl;
		cout << "\t\t\t\t ChatRoom 多人聊天程序 \n";
		cout << "\t\t\t *************【Client】************* \n";
		(void)time(&now);
		timer = ctime(&now);
		cout << "\t\t\t\t" << timer << endl;
		cout << "-----------------------------------------------------------------------------------------------" << endl;
	}
	else
	{
		cout << "init faild" << endl;
	}
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();

	Close();
}

bool Client::Init(CommunicationType type) // 初始化客户端
{
	if (type == TCP_COMMUNICATION)
	{
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);

		if (clientSocket == -1)
		{
			cout << "create socket faild" << endl;
			return false;
		}
		cout << "create socket successed" << endl;

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//10.230.136.55 
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.87");
		addr.sin_port = htons(25000);

		int b;
		b = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));//
		if (b == -1)
		{
			cout << WSAGetLastError() << endl;
			cout << "connect faild" << endl;
			return false;
		}
		cout << "server has been successfully connected, you can send message" << endl;

		return true;
	}
	else
	{
		// TODO: udp
		return false;
	}
}

void Client::Connect()
{
	char str[1024];

	CreateThread(NULL, 0, &receThread, 0, 0, NULL);

	while (1)
	{
		gets_s(str);

		if (strcmp(str, "")) // 非空
		{
			char sendBuffer[1024];
			sprintf(sendBuffer, "%s\n", &str);

			int sendByte = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);
			if (sendByte <= 0)
			{
				cout << "send faild" << endl;
			}
		}
		else
		{
			cout << "invalid input\n";
		}
	}
}

CMessage Client::TypeRecognition(string s)
{
	CMessage message;

	message.pType = PUBLIC_MESSAGE;
	message.mType = TXT_MESSAGE;
	message.roomId = 1;
	message.userId = 1;
	message.mesLength = s.length() + 1;
	strncpy(message.messageBuffer, s.c_str(), s.length() + 1);

	return message;
}

DWORD WINAPI Client::sendThread(LPVOID lpParam)
{
	return 0;
}

DWORD WINAPI Client::receThread(LPVOID lpParam)
{
	char receBuff[1024];

	while (1)
	{
		int recvByte = recv(clientSocket, receBuff, sizeof(receBuff), 0);

		CMessage receiveMessage;

		if (recvByte >0)
		{
			cout << receBuff << endl;
		}
		else
		{
			cout << "receive end" << endl;
			int reRecvByte = recv(clientSocket, receBuff, sizeof(receBuff), 0);
			if (reRecvByte <= 0)
			{
				cout << "break server" << endl;
			}
			break;
		}
	}
	closesocket(clientSocket);
	return 0;
}

void Client::ReConnect()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//10.230.136.55 
	addr.sin_port = htons(25000);

	int b;
	b = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));//
	if (b == -1)
	{
		cout << WSAGetLastError() << endl;
		cout << "connect faild" << endl;
		return;
	}
	cout << "server has been successfully connected, you can send message" << endl;
	return;
}

void Client::Close()
{

}
