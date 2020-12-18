#include "stdafx.h"
#include "Room.h"
#include "TCP.h"
#include "UDP.h"

#define	MAX_ROOM_COUNT	3 // 最大房间数量

static SOCKET clientSocket[1024]; // 所有客户端socket列表

static vector<Room>rooms; // 房间列表

static vector<unsigned>mateIds; // 成员id列表

static HANDLE hThread[100] = { NULL }; // 线程句柄

static char *timer; // 时间

static time_t now; // 表

static SOCKET udpSocket; // udp socket

static SOCKET tcpSocket; // tcp socket

typedef struct ClientInfo{ // 文件发送参数
	unsigned number;
	char sendFile[1024];
}ClientInfo;

static ClientInfo globalClient;

typedef struct ClientParams{ // 客户端参数
	unsigned userId;
	unsigned roomId;
	bool status;
}ClientParams;

typedef enum State { // 登录状态
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
	unsigned count = 0; // 分配用户编号

public:
	Server(CommunicationType type = TCP_COMMUNICATION); // 默认TCP初始化
	~Server();

	void Init(); // 初始化服务器
	void Start(); // 开启服务器
	void ReStart(); // 重启服务器
	void Close(); // 关闭服务器

	static unsigned SyncRoomNumber(unsigned number); // 同步房间号
	static int LoginStatus(unsigned number); // 登录状态机

	static bool EnterRoom(unsigned roomNum, unsigned number); // 进入房间
	static bool CreateRoom(unsigned number); // 创建房间
	static bool FindRoom(unsigned number); // 查找房间
	bool CloseRoom(unsigned number); // 关闭房间
	static bool ChangeRoomNumber(unsigned oldNum, unsigned newNum, unsigned number); // 修改房间号
	static bool FindRoomMate(unsigned roomNum, unsigned number); // 查找成员

	static void delServerMate(ClientParams params); // 从服务器成员列表移除
	static void delRoomMate(ClientParams params); // 从房间成员列表移除

	static DWORD WINAPI RecvFileThread(LPVOID lpParams);  //线程函数
	static DWORD WINAPI SendFileThread(LPVOID lpParams);  //线程函数
	static DWORD WINAPI StartThread(LPVOID lpParams);  //线程函数
	static void TxtTranspond(ClientParams *clientInfo, CMessage* message); // 转发函数
	static void SendMes(char* message, unsigned number); // 指令消息

	// tool
	static bool IsDigital(char str[]); // 识别数字
	static int FuzzyMatch(const char* str); // 模糊匹配
	static CMessage Server::TypeRecognition(string s); // 消息识别
};