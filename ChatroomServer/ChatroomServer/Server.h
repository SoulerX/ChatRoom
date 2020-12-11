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

typedef struct ClientParams{ // �ͻ��˲���
	unsigned userId;
	unsigned roomId;
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
	WSADATA wsaData;
	
	SOCKET severSocket; // ������socket

	unsigned count = 0; // �����û����


public:
	Server(CommunicationType type = TCP_COMMUNICATION); // Ĭ��TCP��ʼ��
	~Server();

	bool Init(CommunicationType type); // ��ʼ��������
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

	static DWORD WINAPI StartThread(LPVOID lpParams);  //�̺߳���
	static void Transpond(CMessage message); // ת������
	static void TxtTranspond(ClientParams *clientInfo, char* message); // ת������
	static void SendMes(char* message, unsigned number); // ָ����Ϣ

	// tool
	static bool IsDigital(char str[]); // ʶ������
	static int FuzzyMatch(char* str); // ģ��ƥ��
};