#include "stdafx.h"
#include "Message.h"

using namespace std;

static SOCKET clientSocket;

static char *timer;

static time_t now;

class Client
{
	WSADATA wsaData;

	unsigned roomId; // 房间 id

	unsigned userId; // 用户 id

	char* mateName; // 用户名

public:

	Client(CommunicationType type = TCP_COMMUNICATION); // 默认TCP初始化
	~Client();

	bool Init(CommunicationType type); // 初始化客户端

	void Connect();
	void ReConnect();
	void Close();

	CMessage TypeRecognition(string s);

	static DWORD WINAPI Client::sendThread(LPVOID lpParam); // 发送
	static DWORD WINAPI Client::receThread(LPVOID lpParam); // 接收
};