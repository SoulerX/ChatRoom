#include "stdafx.h"
#include "Room.h"
#include "TCP.h"
#include "UDP.h"

#define	MAX_ROOM_COUNT	3 // ��󷿼�����

static SOCKET clientSocket[1024]; // ���пͻ���socket�б�

static vector<Room>rooms; // �����б�

static vector<unsigned>mateIds; // ��Աid�б�

static HANDLE hThread[100] = { NULL }; // �߳̾��

static char *timer; // ʱ��

static time_t now; // ��

static SOCKET udpSocket; // udp socket

static SOCKET tcpSocket; // tcp socket

typedef struct ClientInfo{ // �ļ����Ͳ���
	unsigned number;
	char sendFile[1024];
}ClientInfo;

static ClientInfo globalClient;

typedef struct ClientParams{ // �ͻ��˲���
	unsigned userId;
	unsigned roomId;
	bool status;
}ClientParams;

typedef enum State { // ��¼״̬
	InputRB = 0,        
	ExistR,       
	NotExistR,          
	MateEX,       
	RoomEX,
	ChangeRN,
	Successed
}State;

class Server
{
	unsigned count = 0; // �����û����

public:
	Server(CommunicationType type = TCP_COMMUNICATION); // Ĭ��TCP��ʼ��
	~Server();

	void Init(); // ��ʼ��������
	void Start(); // ����������
	void ReStart(); // ����������
	void Close(); // �رշ�����

	static unsigned SyncRoomNumber(unsigned number); // ͬ�������
	static int LoginStatus(unsigned number); // ��¼״̬��

	static bool EnterRoom(unsigned roomNum, unsigned number); // ���뷿��
	static bool CreateRoom(unsigned number); // ��������
	static bool FindRoom(unsigned number); // ���ҷ���
	bool CloseRoom(unsigned number); // �رշ���
	static bool ChangeRoomNumber(unsigned oldNum, unsigned newNum, unsigned number); // �޸ķ����
	static bool FindRoomMate(unsigned roomNum, unsigned number); // ���ҳ�Ա

	static void delServerMate(ClientParams params); // �ӷ�������Ա�б��Ƴ�
	static void delRoomMate(ClientParams params); // �ӷ����Ա�б��Ƴ�

	static DWORD WINAPI RecvFileThread(LPVOID lpParams);  //�̺߳���
	static DWORD WINAPI SendFileThread(LPVOID lpParams);  //�̺߳���
	static DWORD WINAPI StartThread(LPVOID lpParams);  //�̺߳���
	static void TxtTranspond(ClientParams *clientInfo, CMessage* message); // ת������
	static void SendMes(char* message, unsigned number); // ָ����Ϣ

	// tool
	static bool IsDigital(char str[]); // ʶ������
	static int FuzzyMatch(const char* str); // ģ��ƥ��
	static CMessage Server::TypeRecognition(string s); // ��Ϣʶ��
};