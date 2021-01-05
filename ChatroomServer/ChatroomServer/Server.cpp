#include "Server.h"
#include <iostream>

using namespace std;

Server::Server(CommunicationType type)
{
	Init();
	
	cout << "init finished, waiting for connect......\n\n\n" << endl;
	cout << "\t\t ChatRoom 多人聊天程序 \n";
	cout << "\t *************【Server】************* \n";

	(void)time(&now);
	timer = ctime(&now);
	cout << "\t\t" << timer << endl;
	cout << "------------------------------------------------------" << endl;
}

Server::~Server()
{
	Close();
}


// SERVER
void Server::Init() // 初始化服务器
{
	UDP udp;
	udpSocket = udp.udpSocketInit();

	TCP tcp;
	tcpSocket = tcp.tcpSocketInit();
}

void Server::Start() // 开启服务器
{
	//主线程用于接收请求
	while (1)
	{
		if (mateIds.size() > 9) // 控制接入
		{
			continue;
		}

		clientSocket[count++] = accept(tcpSocket, NULL, NULL); // 添加tcp socket
		cout << "Client_" << count << " has been connected server" << endl;
	
		ClientParams newClient;
		newClient.userId = count;

		mateIds.push_back(count); // 将成员id添置至id列表

		char num[10];

		_itoa(mateIds.back(), num, 10);

		char idInfo[30] = "Your id: ";

		strcat(idInfo, num);

		SendMes(idInfo, count);

		strcpy(idInfo, "please input room id: ");

		SendMes(idInfo, count);

		hThread[count - 1] = CreateThread(NULL, 0, StartTcpThread, &newClient, 0, NULL); // 创建线程
		
		Sleep(100);	
	}
}

DWORD WINAPI Server::StartTcpThread(LPVOID lpParams)
{
	ClientParams *temp = (ClientParams*)lpParams;

	ClientParams params;
	params.userId = temp->userId;
	params.roomId = -1;

	while (params.roomId = LoginStatus(params.userId))
	{
		if (params.roomId != -1)
		{
			params.status = true;
			break;
		}
	}

	CMessage recvMes;

	while (1)
	{
		memset(recvMes.messageBuffer, '\0', 64 * BUFSIZE);

		params.roomId = SyncRoomNumber(params.userId); // BUG:发消息前同步房间号， 也许无法第一时间同步

		int receByte = recv(clientSocket[params.userId - 1], (char*)&recvMes, sizeof(CMessage), 0);

		if (receByte == -1 || receByte == 0)
		{
			cout << "exit" << endl;
			params.status = false;
			// 从服务器成员列表移除
			delServerMate(params);
			// 从房间成员列表移除
			delRoomMate(params);

			break;
		}
		transfer(recvMes, params);
	}
		
	// 关闭socket
	closesocket(clientSocket[params.userId - 1]);

	return 0;
}

void Server::transfer(CMessage recvMes, ClientParams params)
{
	if (recvMes.mType == TXT_MESSAGE)
	{
		if (strcmp(recvMes.messageBuffer, "exit") == 0) // 返回大厅
		{
			cout << params.userId << " leave room " << params.roomId << endl;
			// 从房间成员列表移除
			delRoomMate(params);
			params.roomId = -1;
			params.status = false;
			return;
		}
		else if (strcmp(recvMes.messageBuffer, "connect") == 0) // 连接房间
		{
			while (params.roomId = LoginStatus(params.userId))
			{
				cout << params.userId << " connect room " << params.roomId << endl;
				if (params.roomId != -1)
				{
					params.status = true;
					return;
				}
			}
		}
		else if (strcmp(recvMes.messageBuffer, "accept") == 0) // 文件转发
		{
			cout << "send file to user_" << params.userId << endl;
			CreateThread(NULL, 0, &SendFileThread, &params, 0, NULL);

			return;
		}

		fileInfo = FileInfomation(recvMes);
		if (fileInfo->fileSize > 0) // 接收文件
		{			
			string str(fileInfo->fileName);

			int end = str.find_last_of("\\");

			cout << str.substr(end + 1, str.size() - end).c_str() << endl;

			strcpy(globalClient.sendFile, str.substr(end + 1, str.size() - end).c_str());

			fp = fopen(globalClient.sendFile, "wb+");

			cout << "recv file " << fileInfo->fileName << " size " << fileInfo->fileSize << endl;

			starttime = clock();

			return;
		}

		(void)time(&now);
		timer = ctime(&now);
		cout << "time: " << timer << "receive form client_" << params.userId << " message: " << recvMes.messageBuffer << endl;

		if (params.status)
			TxtTranspond(&params, &recvMes);
	}
	else if (recvMes.mType == FILE_MESSAGE) // 接收文件 
	{	
		int ret = 0;

		ret = fwrite(recvMes.messageBuffer, 1, recvMes.mesLength, fp);

		if (ret > 0)
		{
			cout << "recv size " << recvMes.mesLength << endl;
			totalsize += recvMes.mesLength;
			cout << ++record << " total size " << totalsize << endl;
		}
		else
		{
			cout << "error" << endl;
		}

		if (fileInfo->fileSize <= totalsize)
		{
			cout << recvMes.mesLength << endl;
			Sleep(100);
			fclose(fp);
			totalsize = 0;
			record = 0;
			cout << "recv finish" << endl;

			endtime = clock();

			double delta = (double)(endtime - starttime) / CLOCKS_PER_SEC;

			cout << "time: " << delta << " s speed: " << fileInfo->fileSize / delta / 1024 / 1024 << "mb/s" << endl;
			
			char buf[BUFSIZE];

			sprintf(buf, "%s, accept or refuse?\n", fileInfo->fileName);

			memset(recvMes.messageBuffer, '\0', 64 * BUFSIZE);
			strcpy(recvMes.messageBuffer, buf);
			recvMes.mesLength = strlen(buf);
			recvMes.mType = TXT_MESSAGE;
			if (params.status)
				TxtTranspond(&params, &recvMes);
		}
	}
	else if (recvMes.mType == FOLDER_MESSAGE) // 接收文件 
	{
		cout << "don`t support folder" << endl;
	}
}

DWORD WINAPI Server::SendFileThread(LPVOID lpParams)
{
	ClientParams *p = (ClientParams*)lpParams;

	ClientParams params;

	params.userId = p->userId;

	cout << "params.userId: " << params.userId << endl;

	string name(globalClient.sendFile);
	
	int end = name.find_last_of("\\");

	name = name.substr(end + 1, name.size() - end - 1);

	cout << "send file:" << name << endl;

	FILE* fp = fopen(name.c_str(), "rb+");

	if (fp == NULL)
	{
		printf("file open error\n");
		exit(1);
	}
	printf("file open success\n");

	int size = _filelength(_fileno(fp));
	int rsize = size;
	cout << "size:" << size << endl;

	CMessage message;
	memset(message.messageBuffer, '\0', 64 * BUFSIZE);

	clock_t starttime, endtime;
	starttime = clock();

	int i = 0;
	while (size > 0) {
		memset(message.messageBuffer, '\0', 64 * BUFSIZE);
		message.mType = FILE_MESSAGE;

		int temp = (size < 64 * BUFSIZE ? size : 64 * BUFSIZE);

		message.mesLength = temp;

		fread(message.messageBuffer, 1, temp, fp);

		int ret = 0;
		if ((ret = send(clientSocket[params.userId-1], (char*)&message, sizeof(CMessage), 0)) == -1)
		{
			cout << "send error" << endl;
			break;
		}
		else
		{
			size -= temp;
			if (temp < 64 * BUFSIZE)
			{
				cout << "send finish" << endl;

				endtime = clock();

				double delta = (double)(endtime - starttime) / CLOCKS_PER_SEC;

				printf("time = %f s  speed = %f mb/s\n", delta, rsize / delta / 1024 / 1024);

				fclose(fp);
			}
			else
			{
				cout << ++i << " residue size " << size << endl;
			}
		}
		//Sleep(10);
	}

	return 0;
}

unsigned Server::SyncRoomNumber(unsigned number) // 同步房间号
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); iter++){
		for (vector<unsigned>::iterator iter2 = iter->mateList.begin(); iter2 != iter->mateList.end(); iter2++){
			if (*iter2 == number)
				return iter->roomNumber;
		}
	}
	return -1;
}

int Server::LoginStatus(unsigned number)
{
	State status = InputRB;

	SendMes("please input room number:", number);

	CMessage recvMes;

	unsigned roomNum;

	while (1){
		int receByte = recv(clientSocket[number - 1], (char*)&recvMes, sizeof(CMessage), 0);
		if (receByte == -1)
		{
			continue;
		}
		else
		{
			switch (status)
			{
			case InputRB:
				if (!IsDigital(recvMes.messageBuffer)) // 输入有误
				{
					SendMes("your input error, please reinput room number:", number);	
				}
				else
				{
					roomNum = (unsigned)atoi(recvMes.messageBuffer);
					if (FindRoom(roomNum)) // 发现房间号
					{
						SendMes("room exist, enter room y/n or change?", number);
						status = ExistR;
					}
					else  // 未发现房间号
					{
						char str[128];
						sprintf(str, "room not exist, do you want to create room_%d y/n?", roomNum);
						SendMes(str, number);
						status = NotExistR;
					}
				}
				break;
			case ExistR:
				if (strcmp(recvMes.messageBuffer, "y") == 0 || strcmp(recvMes.messageBuffer, "n") == 0 || strcmp(recvMes.messageBuffer, "Y") == 0 || strcmp(recvMes.messageBuffer, "N") == 0)
				{
					if (strcmp(recvMes.messageBuffer, "y") == 0 || strcmp(recvMes.messageBuffer, "Y") == 0)
					{
						if (EnterRoom(roomNum, number))
						{
							status = Successed;
							return roomNum;
						}
						else
						{
							SendMes("room mate full, please reinput room number:", number);
							status = InputRB;
						}
					}
					else // 不进入
					{
						SendMes("please input room number:", number);
						status = InputRB;
					}
				}
				else if (strcmp(recvMes.messageBuffer, "change") == 0) // 修改房间号（名称）
				{
					SendMes("please input new room number:", number);
					status = ChangeRN;
				}
				else // 输入有误
				{
					SendMes("input error, enter room y/n?", number);
				}
				break;
			case NotExistR:
				if (strcmp(recvMes.messageBuffer, "y") == 0 || strcmp(recvMes.messageBuffer, "n") == 0 || strcmp(recvMes.messageBuffer, "Y") == 0 || strcmp(recvMes.messageBuffer, "N") == 0)
				{
					if (strcmp(recvMes.messageBuffer, "y") == 0 || strcmp(recvMes.messageBuffer, "Y") == 0)
					{
						if (rooms.size() >= 3)
						{
							SendMes("room full", number);
							status = InputRB;
							break;
						}

						if (CreateRoom(roomNum)) // 创建成功
						{
							if (EnterRoom(roomNum, number)) // 进入成功
							{
								status = Successed;
								return roomNum;
							}
							else
							{
								SendMes("room mate full, please reinput room number:", number);
								status = InputRB;
							}
						}
						else // 房间已存在
						{
							char str[128];
							sprintf(str, "room_%d has been exist,enter room y/n? ", roomNum);
							SendMes(str, number);
							status = ExistR;
						}
					}
					else // 不进入
					{
						SendMes("please input room number:", number);
						status = InputRB;
					}
				}
				else // 输入有误
				{
					char str[128];
					sprintf(str, "input error, create room_%d y/n?", roomNum);
					SendMes(str, number);
				}
				break;
			case ChangeRN:
				if (IsDigital(recvMes.messageBuffer))
				{
					unsigned newNum = (unsigned)atoi(recvMes.messageBuffer);

					if (ChangeRoomNumber(roomNum, newNum, number))
					{
						cout << "old room num:" << roomNum << " ===>> new room num:" << newNum << endl;
						roomNum = newNum;
						SendMes("change room name, enter room y/n or change?", number);
						status = ExistR;
					}
				}
				else
				{
					char str[128];
					sprintf(str, "input error, please input new room number:");
					SendMes(str, number);
				}
				break;
			default:
				break;
			}
		}
	}
	return -1;
}

void Server::SendMes(char* message, unsigned number)
{
	char sendBuffer[BUFSIZE];

	sprintf(sendBuffer, "server=> %s", message);

	CMessage mes;
	memset(mes.messageBuffer, '\0', 64 * BUFSIZE);
	mes.mType = TXT_MESSAGE;
	strcpy(mes.messageBuffer, sendBuffer);

	

	int sendByte = send(clientSocket[number - 1], (char*)&mes, sizeof(CMessage), 0);//返回发送数据的总和
	if (sendByte < 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		//cout << "send message to client_" << number << " successed:" << message << endl;
	}
}

void Server::TxtTranspond(ClientParams *clientInfo, CMessage* message) // 转发函数
{
	for (vector<Room>::iterator iter1 = rooms.begin(); iter1 != rooms.end();iter1++)
	{ // 迭代匹配用户
		if (iter1->roomNumber == clientInfo->roomId)
		{
			CMessage mes;
			memset(mes.messageBuffer, '\0', 64 * BUFSIZE);
			strcpy(mes.messageBuffer, message->messageBuffer);
			mes.mesLength = message->mesLength;
			mes.mType = message->mType;

			for (vector<unsigned>::iterator iter = iter1->mateList.begin(); iter != iter1->mateList.end(); iter++)
			{
				char sendBuffer[1024];
				sprintf(sendBuffer, "client_%d=> %s\n", clientInfo->userId, message->messageBuffer);
				
				
				memset(mes.messageBuffer, '\0', 64 * BUFSIZE);
				strcpy(mes.messageBuffer, sendBuffer);
				mes.mesLength = strlen(sendBuffer);
				mes.mType = message->mType;
				
				if (message->pType == PUBLIC_MESSAGE && *iter != clientInfo->userId) // 公开发送
				{
					//int sendByte = send(clientSocket[*iter - 1], sendBuffer, strlen(sendBuffer), 0);//返回发送数据的总和		

					int sendByte = send(clientSocket[*iter - 1], (char*)&mes, sizeof(CMessage), 0);//返回发送数据的总和
					if (sendByte < 0)
					{
						cout << "send faild" << endl;
					}
					else
					{
						//cout << "transpond message to client_" << *iter << " successed: " << sendBuffer;
					}
				}
				else if (message->pType == PRIVATE_MESSAGE && *iter == message->sendObjId) // 单独发送
				{
					if (message->sendObjId == clientInfo->userId)
					{
						char sendBuffer[30];
						sprintf(sendBuffer, "forbid send to yourself");

						memset(mes.messageBuffer, '\0', 64 * BUFSIZE);
						strcpy(mes.messageBuffer, sendBuffer);
						mes.mesLength = message->mesLength;
						mes.mType = message->mType;


						int sendByte = send(clientSocket[*iter - 1], (char*)&mes, sizeof(CMessage), 0);//返回发送数据的总和
						if (sendByte < 0)
						{
							cout << "send faild" << endl;
						}
						else
						{
							cout << "forbid send to yourself\n";
						}
					}
					else
					{
						int sendByte = send(clientSocket[*iter - 1], (char*)&mes, sizeof(CMessage), 0);//返回发送数据的总和
						if (sendByte < 0)
						{
							cout << "send faild" << endl;
						}
						else
						{
							//cout << "single send message to client_" << *iter << " successed:" << message->messageBuffer;
						}
					}
					break;
				}
			}
		}
	}
}

void Server::delServerMate(ClientParams params) // 从服务器成员列表移除
{
	for (vector<unsigned>::iterator iter = mateIds.begin(); iter != mateIds.end(); ++iter){
		if (*iter == params.userId)
		{
			mateIds.erase(iter);
			break;
		}
	}
}

void Server::ReStart() // 重启服务器
{

}

void Server::Close() // 关闭服务器
{
	closesocket(tcpSocket);
	WSACleanup();
}


// ROOM
bool Server::FindRoomMate(unsigned roomNum, unsigned number) // 查找成员
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == roomNum)
		{
			for (vector<unsigned>::iterator iter2 = iter->mateList.begin(); iter2 != iter->mateList.end(); ++iter2){
				if (*iter2 == number)
				{
					return true;
				}
			}
			char sendBuffer[128];
			sprintf(sendBuffer, "don`t find user_%d\n", number);
			SendMes(sendBuffer, number);
			return false;
		}
	}
	return false;
}

void Server::delRoomMate(ClientParams params) // 从房间成员列表移除
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == params.roomId)
		{
			for (vector<unsigned>::iterator iter2 = iter->mateList.begin(); iter2 != iter->mateList.end(); ++iter2){
				if (*iter2 == params.userId)
				{
					iter->OutChatroom(*iter2);

					char leave[64];
					sprintf(leave, "<<=== leave room");

					CMessage delMes;

					strcpy(delMes.messageBuffer, leave);
					delMes.mesLength = sizeof(leave);

					TxtTranspond(&params, &delMes);

					return;
				}
			}
		}
	}
}

bool Server::EnterRoom(unsigned roomNum, unsigned number) // 进入房间
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){ 
		if (iter->roomNumber == roomNum){
			if (iter->GetMateCount() < 3)
			{
				iter->InChatroom(number);

				char enter[64];
				sprintf(enter, " ===>> enter room");

				CMessage enterMes;

				strcpy(enterMes.messageBuffer, enter);
				enterMes.mesLength = sizeof(enter);

				ClientParams params;
				params.userId = number;
				params.roomId = roomNum;

				TxtTranspond(&params, &enterMes);

				char temp2[30];
				sprintf(temp2, "you enter room(%d)", roomNum);
				SendMes(temp2, number);

				return true;
			}
			return false;
		}
	}
	return false;
}

bool Server::CreateRoom(unsigned number) // 创建房间
{
	Room room(number);
	rooms.push_back(room);
	cout << "create room_" << number << endl;
	return true;
}

bool Server::FindRoom(unsigned number) // 查找房间
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == number){
			return true;
		}
	}
	return false;
}

bool Server::CloseRoom(unsigned number) // 关闭房间
{
	return true;
}

bool Server::ChangeRoomNumber(unsigned oldNum, unsigned newNum, unsigned number) // 修改房间号
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == newNum){
			cout << "room number conflict" << endl;
			SendMes("room number conflict", number);
			return false;
		}
	}
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == oldNum){
			iter->roomNumber = newNum;
			return true;
		}
	}
	return false;
}


// TOOL
bool Server::IsDigital(char str[]) // 识别数字
{
	while (*str){
		if ((*str < '0' || *str > '9')&&*str!='\n')
		{
			return false;
		}
		*str++;
	}
	return true;
}

int Server::FuzzyMatch(const char* str) // 模糊匹配
{
	string s = str;
	unsigned start = s.find("/-");
	unsigned end = s.find("-/");
	char temp[10];

	if (start == 0 && end < 10000 && end - start>2)
	{
		strncpy(temp, s.substr(start + 2, end - 2).c_str(), end - 2 + 1);
		if (IsDigital(temp))
		{
			return atoi(temp);
		}
	}
	return -1;
}

CMessage Server::TypeRecognition(string s)
{
	CMessage message;

	message.sendObjId = FuzzyMatch(s.c_str());

	string newStr;
	if (-1 != message.sendObjId)
	{
		int start = s.find("-/");

		message.mesLength = s.length() + 1 - start + 2;

		newStr = s.substr(start + 2, message.mesLength);

		strncpy(message.messageBuffer, newStr.c_str(), newStr.length() + 1);

		message.pType = PRIVATE_MESSAGE;
	}
	else
	{
		message.mesLength = s.length() + 1;
		strncpy(message.messageBuffer, s.c_str(), s.length() + 1);
		message.pType = PUBLIC_MESSAGE;

		newStr = s;
	}

	if (s.find(":/") >= 0 && s.find(":/") < 10000 || s.find(":\\") >= 0 && s.find(":\\") < 10000)
	{
		message.mType = FileRecognition(newStr);
	}
	else
	{
		message.mType = TXT_MESSAGE;
	}

	return message;
}

MessageType Server::FileRecognition(string s) // 文件识别
{
	WIN32_FIND_DATAA FindFileData;

	FindFirstFileA(s.c_str(), &FindFileData);

	if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		return FOLDER_MESSAGE;
	else
		return FILE_MESSAGE;
}

FileInfo* Server::FileInfomation(CMessage message)
{
	FileInfo *info = new FileInfo;

	info->fileSize = 0;

	string str(message.messageBuffer);

	cout << str.substr(0, 4).c_str() << endl;
	if (strcmp(str.substr(0, 4).c_str(), "file") == 0)
	{
		int end = str.find_last_of(" ");
		cout << "filename:" << str.substr(5, end - 4).c_str() << endl;
		strcpy(info->fileName, str.substr(5, end - 4).c_str());
		cout << "filesize:" << str.substr(end+1, str.size() - end - 1).c_str()<< endl;
		info->fileSize = atoi(str.substr(end, str.size() - end).c_str());
	}

	return info;
}
