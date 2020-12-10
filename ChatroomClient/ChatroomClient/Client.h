#include "stdafx.h"
#include "Message.h"

using namespace std;

static SOCKET clientSocket;

static char *timer;

static time_t now;

class Client
{
	WSADATA wsaData;

	unsigned roomId; // ���� id

	unsigned userId; // �û� id

	char* mateName; // �û���

public:

	Client(CommunicationType type = TCP_COMMUNICATION); // Ĭ��TCP��ʼ��
	~Client();

	bool Init(CommunicationType type); // ��ʼ���ͻ���

	void Connect();
	void ReConnect();
	void Close();

	CMessage TypeRecognition(string s);

	static DWORD WINAPI Client::sendThread(LPVOID lpParam); // ����
	static DWORD WINAPI Client::receThread(LPVOID lpParam); // ����
};