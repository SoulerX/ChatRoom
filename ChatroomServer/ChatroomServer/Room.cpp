#include "Room.h"

using namespace std;

Room::Room(unsigned num)
{
	this->roomNumber = num;
}

Room::~Room()
{

}

unsigned Room::InChatroom(unsigned mateId) // ����������
{
	mateList.push_back(mateId);
	cout << mateId << " - enter room(" << roomNumber <<") - "<< roomName << endl << endl;
	return roomNumber;
}

void Room::OutChatroom(unsigned mateId) // �뿪������
{
	for (vector<unsigned>::iterator iter = mateList.begin(); iter != mateList.end(); ++iter)
	{
		if (mateId == *iter){
			mateList.erase(iter);
			cout << mateId << " - leave room(" << roomNumber << ")\n" << endl;
			break;
		}
	}
}

unsigned Room::GetMateCount() // ��ȡ�����ҳ�Ա����
{
	return this->mateList.size();
}

void Room::SetChatroomTitle(const char* newRoomName) // �޸������ұ���
{
	this->roomName = (char *)newRoomName;
	cout << "the room name has been changed to - " << newRoomName << endl;
}

bool Room::FindMate(unsigned mateId) // ���ҳ�Ա
{
	for (vector<unsigned>::iterator iter = mateList.begin(); iter != mateList.end(); ++iter)
	{
		if (mateId == *iter){
			cout << "user_" << mateId << " has been exist." << endl;
			return true;
		}
	}
	return false;
}
