#include "UDP.h"
#include "TCP.h"


using namespace std;

static SOCKET tcpSocket; // tcp

static SOCKET udpSocket; // udp

static sockaddr_in serAddr; // udp addr

static char *timer; // ��ʱ��

static time_t now; // ��ʱ

static vector<string>fileName; // �ļ���

static bool rf; // �ռ�����

class Client
{
	WSADATA wsaData;

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