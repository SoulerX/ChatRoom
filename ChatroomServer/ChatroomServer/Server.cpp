#include "Server.h"

Server::Server(CommunicationType type)
{
	// 版本检查
	WORD require;
	require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)//成功加载返回0；
	{
		cout << "loading winsock faild" << endl;
		return;
	}

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "request version faild" << endl;
		return;
	}

	cout << "request version successed" << endl;

	if (Init(type))
	{
		cout << "init finished, waiting for connect......\n\n\n" << endl;

		cout << "\t\t\t\t ChatRoom 多人聊天程序 \n";
		cout << "\t\t\t *************【Server】************* \n";
		(void)time(&now);
		timer = ctime(&now);
		cout << "\t\t\t\t" << timer << endl;
		cout << "-----------------------------------------------------------------------------------------------" << endl;
	}
	else{
		cout << "init faild" << endl;
	}
}

Server::~Server()
{
	Close();
}

// SERVER
bool Server::Init(CommunicationType type) // 初始化服务器
{
	if (TCP_COMMUNICATION == type)
	{
		// 创建SOCKET
		severSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (severSocket == -1)
		{
			cout << "create socket faild" << endl;
			return false;
		}
		cout << "create socket successed" << endl;

		// 配置协议
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.87");
		addr.sin_port = htons(25000);

		int result; // 返回值

		// 绑定
		result = bind(severSocket, (sockaddr*)&addr, sizeof(addr));
		if (result == -1)
		{
			cout << "bind socket faild" << endl;
			return false;
		}
		cout << "bind socket successed" << endl;

		// 监听
		result = listen(severSocket, 10);
		if (result == -1)
		{
			cout << "listen faild" << endl;
			return false;
		}
		cout << "listen successed" << endl;

		return true;
	}
	else
	{
		// TODO: udp
		return false;
	}
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

		clientSocket[count++] = accept(severSocket, NULL, NULL); // 添加socket
		cout << "Client_" << count << " has been connected server" << endl;
	
		ClientParams newClient;
		newClient.userId = count;

		mateIds.push_back(count); // 将成员id添置至id列表

		hThread[count - 1] = CreateThread(NULL, 0, StartThread, &newClient, 0, NULL); // 创建线程
		Sleep(100);	
	}
}

DWORD WINAPI Server::StartThread(LPVOID lpParams)
{
	ClientParams *temp = (ClientParams*)lpParams;

	ClientParams params;
	params.userId = temp->userId;
	params.roomId = -1;

	while (params.roomId = LoginStatus(params.userId))
	{
		if (params.roomId != -1)
			break;
	}

	char lpBuffer[1024];
	while (1)
	{
		int receByte = recv(clientSocket[params.userId - 1], lpBuffer, sizeof(lpBuffer), 0);
		if (receByte == -1)
		{
			int reReceByte = recv(clientSocket[params.userId - 1], lpBuffer, sizeof(lpBuffer), 0);
			if (reReceByte == -1)
			{
				// 从服务器成员列表移除
				delServerMate(params);
				// 从房间成员列表移除
				delRoomMate(params);
				break;
			}
			continue;
		}
		else
		{
			if (strcmp(lpBuffer, "exit\n") == 0) // 返回大厅
			{
				// 从房间成员列表移除
				delRoomMate(params);
				params.roomId = -1;
			}
			else if (strcmp(lpBuffer, "connect\n") == 0) // 连接房间
			{
				while (params.roomId = LoginStatus(params.userId))
				{
					if (params.roomId != -1)
					{
						break;
					}
				}
			}
				
			(void)time(&now);
			timer = ctime(&now);
			cout << "time: " << timer << "receive form client_" << params.userId << " message: " << lpBuffer << endl;

			params.roomId = SyncRoomNumber(params.userId);

			TxtTranspond(&params, lpBuffer);
		}
	}

	// 关闭socket
	closesocket(clientSocket[params.userId - 1]);

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

	char lpBuffer[1024];

	unsigned roomNum;
	
	while (1){
		int receByte = recv(clientSocket[number - 1], lpBuffer, sizeof(lpBuffer), 0);
		if (receByte == -1)
		{
			continue;
		}
		else
		{
			switch (status)
			{
			case InputRB:
				if (!IsDigital(lpBuffer)) // 输入有误
				{
					SendMes("your input error, please reinput room number:", number);
				}
				else
				{
					roomNum = (unsigned)atoi(lpBuffer);
					if (FindRoom(roomNum)) // 发现房间号
					{
						SendMes("room exist, enter room y/n or change?", number);
						status = ExistR;
					}
					else  // 未发现房间号
					{
						char str[128];
						sprintf(str, "room not exist, do yu want to create room_%d y/n?", roomNum);
						SendMes(str, number);
						status = NotExistR;
					}
				}
				break;
			case ExistR:
				if (strcmp(lpBuffer, "y\n")==0 || strcmp(lpBuffer, "n\n")==0 || strcmp(lpBuffer, "Y\n")==0 || strcmp(lpBuffer, "N\n")==0)
				{
					if (strcmp(lpBuffer, "y\n")==0 || strcmp(lpBuffer, "Y\n")==0)
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
				else if (strcmp(lpBuffer, "change\n")==0) // 修改房间号（名称）
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
				if (strcmp(lpBuffer, "y\n") == 0 || strcmp(lpBuffer, "n\n") == 0 || strcmp(lpBuffer, "Y\n") == 0 || strcmp(lpBuffer, "N\n") == 0)
				{
					if (strcmp(lpBuffer, "y\n") == 0 || strcmp(lpBuffer, "Y\n") == 0)
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
				if (IsDigital(lpBuffer))
				{
					unsigned newNum = (unsigned)atoi(lpBuffer);

					if (ChangeRoomNumber(roomNum, newNum))
					{
						cout << "old room num:" << roomNum << " ===>> new room num:" << newNum <<endl;
						roomNum = newNum;
						SendMes("room exist, enter room y/n or change?", number);
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
	char sendBuffer[128];

	sprintf(sendBuffer, "%s", message);

	int sendByte = send(clientSocket[number - 1], sendBuffer, sizeof(sendBuffer), 0);//返回发送数据的总和
	if (sendByte < 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		//cout << "send message to client_" << number << " successed:" << message << endl;
	}
}

void Server::TxtTranspond(ClientParams *clientInfo, char* message) // 转发函数
{
	char sendBuffer[1024];

	sprintf(sendBuffer, "time: %sroom_%d-client_%d: %s\n", timer, clientInfo->roomId, clientInfo->userId, message);

	PublicType publicType = PUBLIC_MESSAGE;
	unsigned sendObj = 1;

	for (vector<Room>::iterator iter1 = rooms.begin(); iter1 != rooms.end();iter1++)
	{ // 迭代匹配用户
		if (iter1->roomNumber == clientInfo->roomId)
		{
			for (vector<unsigned>::iterator iter = iter1->mateList.begin(); iter != iter1->mateList.end(); iter++)
			{
				if (publicType == PUBLIC_MESSAGE && *iter != clientInfo->userId) // 公开发送
				{
					int sendByte = send(clientSocket[*iter - 1], sendBuffer, sizeof(sendBuffer), 0);//返回发送数据的总和
					if (sendByte < 0)
					{
						cout << "send faild" << endl;
					}
					else
					{
						//cout << "send message to client_" << *iter << " successed:" << message;
					}
				}
				else if (publicType == PRIVATE_MESSAGE && *iter == sendObj) // 单独发送
				{
					int sendByte = send(clientSocket[clientInfo->userId - 1], sendBuffer, sizeof(sendBuffer), 0);//返回发送数据的总和
					if (sendByte < 0)
					{
						cout << "send faild" << endl;
					}
					else
					{
						//cout << "send message to client_" << *iter << " successed:" << message;
					}
					break;
				}
			}
		}
	}
}

void Server::Transpond(CMessage message) // 转发函数
{
	
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
	closesocket(severSocket);
	WSACleanup();
}


// ROOM
void Server::delRoomMate(ClientParams params) // 从房间成员列表移除
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == params.roomId)
		{
			for (vector<unsigned>::iterator iter2 = iter->mateList.begin(); iter2 != iter->mateList.end(); ++iter2){
				if (*iter2 == params.userId)
				{
					iter->OutChatroom(*iter2);

					char leave[30] = " - leave room ";
					char temp[10];
					_itoa(iter->roomNumber, temp, 10);
					strcat(leave, temp);
					strcat(leave, "\n");
					TxtTranspond(&params, leave);

					break;
				}
			}
		}
	}
}

bool Server::CreateRoom(unsigned number) // 创建房间
{
	Room room(number);
	rooms.push_back(room);

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

bool Server::EnterRoom(unsigned roomNum, unsigned number) // 进入房间
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){ 
		if (iter->roomNumber == roomNum){
			if (iter->GetMateCount() < 3)
			{
				iter->InChatroom(number);

				char enter[30] = " - enter room ";
				char temp[10];
				_itoa(iter->roomNumber, temp, 10);
				strcat(enter, temp);
				strcat(enter, "\n");

				ClientParams params;
				params.userId = number;
				params.roomId = roomNum;

				TxtTranspond(&params, enter);

				return true;
			}
			return false;
		}
	}
	return false;
}

bool Server::CloseRoom(unsigned number) // 关闭房间
{
	return true;
}

bool Server::ChangeRoomNumber(unsigned oldNum, unsigned newNum) // 修改房间号
{
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
