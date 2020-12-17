#include "UDP.h"
#include "TCP.h"


using namespace std;

static SOCKET tcpSocket;

static SOCKET udpSocket;

static sockaddr_in serAddr;

static char *timer;

static time_t now;

static vector<string>fileName;

static bool rf;

class Client
{
	WSADATA wsaData;

	unsigned roomId; // 房间 id

	unsigned userId; // 用户 id

	char* mateName; // 用户名

	UDP udp;

	TCP tcp;

	

public:

	Client(); // 默认TCP初始化
	~Client();

	void Init(); // 初始化客户端

	void Connect();
	void ReConnect();
	void Close();

	CMessage TypeRecognition(string s);

	static DWORD WINAPI Client::sendThread(LPVOID lpParam); // 发送
	static DWORD WINAPI Client::receThread(LPVOID lpParam); // 接收
	static DWORD WINAPI Client::SendFile(LPVOID lpParam); // 发送文件
	static DWORD WINAPI Client::RecvFile(LPVOID lpParams);
	static void SendMes(char* message);

	// tool
	bool IsDigital(char str[]); // 识别数字
	int FuzzyMatch(const char* str); // 模糊匹配
};