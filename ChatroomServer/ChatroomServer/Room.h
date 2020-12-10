#include "stdafx.h"
#include "Message.h"

using namespace std;

#define	MAX_ROOMMATE_COUNT	3 // ����Ա����


class Room
{
	CommunicationType ctype; // ͨѶ����

	vector<CMessage> historyMessage; // ��Ϣ��ʷ

public:
	unsigned roomNumber; // �����ұ��
	char* roomName = "room_default"; // Ĭ����������
	vector<unsigned> mateList; // ��Ա��Ϣ�б�
	
	Room(unsigned num);
	~Room();

	unsigned InChatroom(unsigned mateId); // ����������

	void OutChatroom(unsigned mateId); // �뿪������

	unsigned GetMateCount(); // ��ȡ�����ҳ�Ա����

	void SetChatroomTitle(const char* newRoomName); // �޸������ұ���

	bool FindMate(unsigned mateId); // ���ҳ�Ա

};