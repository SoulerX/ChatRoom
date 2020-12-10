#include "stdafx.h"
#include "Message.h"

using namespace std;

#define	MAX_ROOMMATE_COUNT	3 // 最大成员数量


class Room
{
	CommunicationType ctype; // 通讯类型

	vector<CMessage> historyMessage; // 消息历史

public:
	unsigned roomNumber; // 聊天室编号
	char* roomName = "room_default"; // 默认聊天室名
	vector<unsigned> mateList; // 成员信息列表
	
	Room(unsigned num);
	~Room();

	unsigned InChatroom(unsigned mateId); // 进入聊天室

	void OutChatroom(unsigned mateId); // 离开聊天室

	unsigned GetMateCount(); // 获取聊天室成员数量

	void SetChatroomTitle(const char* newRoomName); // 修改聊天室标题

	bool FindMate(unsigned mateId); // 查找成员

};