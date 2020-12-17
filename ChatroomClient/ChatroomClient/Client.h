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

	unsigned roomId; // ���� id

	unsigned userId; // �û� id

	char* mateName; // �û���

	UDP udp;

	TCP tcp;

	

public:

	Client(); // Ĭ��TCP��ʼ��
	~Client();

	void Init(); // ��ʼ���ͻ���

	void Connect();
	void ReConnect();
	void Close();

	CMessage TypeRecognition(string s);

	static DWORD WINAPI Client::sendThread(LPVOID lpParam); // ����
	static DWORD WINAPI Client::receThread(LPVOID lpParam); // ����
	static DWORD WINAPI Client::SendFile(LPVOID lpParam); // �����ļ�
	static DWORD WINAPI Client::RecvFile(LPVOID lpParams);
	static void SendMes(char* message);

	// tool
	bool IsDigital(char str[]); // ʶ������
	int FuzzyMatch(const char* str); // ģ��ƥ��
};