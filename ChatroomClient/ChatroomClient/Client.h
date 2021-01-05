#include "UDP.h"
#include "TCP.h"


using namespace std;

static SOCKET tcpSocket; // tcp

static SOCKET udpSocket; // udp

static sockaddr_in serAddr; // udp addr

static char *timer; // 计时器

static time_t now; // 计时

static vector<string>fileName; // 文件名

static bool rf; // 收件开关

static FILE* fp;

static int totalsize;

static int record;

static clock_t starttime;

static clock_t endtime;

class Client
{
	WSADATA wsaData;

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

	static DWORD WINAPI sendThread(LPVOID lpParam); // 发送
	static DWORD WINAPI receThread(LPVOID lpParam); // 接收
	static DWORD WINAPI SendFile(LPVOID lpParam); // 发送文件
	static DWORD WINAPI RecvFile(LPVOID lpParams);
	static void SendMes(char* message);

	// tool
	bool IsDigital(char str[]); // 识别数字
	int FuzzyMatch(const char* str); // 模糊匹配
};