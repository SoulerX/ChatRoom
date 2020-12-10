#include "Room.h"

using namespace std;

Room::Room(unsigned num)
{
	this->roomNumber = num;
}

Room::~Room()
{

}

unsigned Room::InChatroom(unsigned mateId) // 进入聊天室
{
	mateList.push_back(mateId);
	cout << mateId << " - enter room(" << roomNumber <<") - "<< roomName << endl << endl;
	return roomNumber;
}

void Room::OutChatroom(unsigned mateId) // 离开聊天室
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

unsigned Room::GetMateCount() // 获取聊天室成员数量
{
	return this->mateList.size();
}

void Room::SetChatroomTitle(const char* newRoomName) // 修改聊天室标题
{
	this->roomName = (char *)newRoomName;
	cout << "the room name has been changed to - " << newRoomName << endl;
}

bool Room::FindMate(unsigned mateId) // 查找成员
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
