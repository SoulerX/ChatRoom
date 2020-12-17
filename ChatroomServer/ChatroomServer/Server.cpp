#include "Server.h"
#include <iostream>

using namespace std;

Server::Server(CommunicationType type)
{
	// �汾���
	WORD require;
	require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)//�ɹ����ط���0��
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

		cout << "\t\t\t\t ChatRoom ����������� \n";
		cout << "\t\t\t *************��Server��************* \n";
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
bool Server::Init(CommunicationType type) // ��ʼ��������
{
	UDP udp;

	fd = udp.serverSocketInit();

	if (TCP_COMMUNICATION == type)
	{
		// ����SOCKET
		severSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (severSocket == -1)
		{
			cout << "create socket faild" << endl;
			return false;
		}
		cout << "create socket successed" << endl;

		// ����Э��
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_addr.S_un.S_addr = INADDR_ANY;
		//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.87");
		addr.sin_port = htons(25000);

		int result; // ����ֵ

		// ��
		result = bind(severSocket, (sockaddr*)&addr, sizeof(addr));
		if (result == -1)
		{
			cout << "bind socket faild" << endl;
			return false;
		}
		cout << "bind socket successed" << endl;

		// ����
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

void Server::Start() // ����������
{
	//���߳����ڽ�������
	while (1)
	{
		if (mateIds.size() > 9) // ���ƽ���
		{
			continue;
		}

		clientSocket[count++] = accept(severSocket, NULL, NULL); // ���socket
		cout << "Client_" << count << " has been connected server" << endl;
	
		ClientParams newClient;
		newClient.userId = count;

		mateIds.push_back(count); // ����Աid������id�б�

		hThread[count - 1] = CreateThread(NULL, 0, StartThread, &newClient, 0, NULL); // �����߳�
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
		{
			params.status = true;
			break;
		}
	}

	CMessage recvMes;

	char fileName[128];

	bool ac = true;

	while (1)
	{
		params.roomId = SyncRoomNumber(params.userId); // BUG:����Ϣǰͬ������ţ� Ҳ���޷���һʱ��ͬ��
		int receByte = recv(clientSocket[params.userId - 1], (char*)&recvMes, sizeof(CMessage), 0);
		if (receByte == -1)
		{
			int reReceByte = recv(clientSocket[params.userId - 1], (char*)&recvMes, sizeof(CMessage), 0);
			if (reReceByte == -1)
			{
				params.status = false;
				// �ӷ�������Ա�б��Ƴ�
				delServerMate(params);
				// �ӷ����Ա�б��Ƴ�
				delRoomMate(params);

				break;
			}
			continue;
		}
		else
		{
			if (strcmp(recvMes.messageBuffer, "exit") == 0) // ���ش���
			{
				// �ӷ����Ա�б��Ƴ�
				delRoomMate(params);
				params.roomId = -1;
				params.status = false;
			}
			else if (strcmp(recvMes.messageBuffer, "connect") == 0) // ���ӷ���
			{
				while (params.roomId = LoginStatus(params.userId))
				{
					cout << "roomid:" << params.roomId << endl;
					if (params.roomId != -1)
					{
						params.status = true;
						break;
					}
				}
			}
			else if (strcmp(recvMes.messageBuffer, "accept") == 0 && ac) // �ļ�ת��
			{
				if (1)
				{
					cout << "send file to user_" << params.userId << endl;
					
					CreateThread(NULL, 0, &SendFileThread, 0, 0, NULL);
					continue;
				}
			}
			else if (recvMes.mType == FILE_MESSAGE) // �ļ�ת����Ȩ
			{
				string s = recvMes.messageBuffer;
				int start = s.find_last_of("\\");
				strncpy(fileName, s.substr(start + 1, s.length() - start).c_str(), s.length() - start + 1);
				char temp[128];
				sprintf(temp, "%s, accept or refuse?", fileName);
				strcpy(recvMes.messageBuffer, temp);
				recvMes.mesLength = sizeof(recvMes.messageBuffer);

				char sendBuffer[2000] = "accept1";
				SendMes(sendBuffer, params.userId);
			
				globalClient.number = params.userId;
				strcpy(globalClient.sendFile, fileName);

				CreateThread(NULL, 0, RecvFileThread, 0, 0, NULL); // ���ش洢

				ac = false;
			}

			(void)time(&now);
			timer = ctime(&now);
			cout << "time: " << timer << "receive form client_" << params.userId << " message: " << recvMes.messageBuffer << endl;

			if (params.status)
				TxtTranspond(&params, &recvMes);
		}
	}

	// �ر�socket
	closesocket(clientSocket[params.userId - 1]);

	return 0;
}

DWORD WINAPI Server::RecvFileThread(LPVOID lpParams)
{
	struct sockaddr_in* p_remoteAddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
	PackInfo pack = { 0, 0, { 0 } };
	BackInfo back;


	printf("waiting for message...\n");
	int ret = 0;

	back.id = 1;
	pack.id = 0;
	if ((ret = UDP::serverRecvMessage(&pack, fd, p_remoteAddr)) == -1)
	{
		printf("the first package recv error");
		exit(1);
	}
	else
	{
		printf(" %d-%d\n", pack.id,strlen(pack.buf));
		printf("success recv pack id:%d\n", pack.id);	
	}

	FILE* fp = fopen(globalClient.sendFile, "wb+");
	fwrite(pack.buf, 1, BUFSIZE - 1, fp);

	while (1)
	{

		if (UDP::serverSendMessage(&back, fd, p_remoteAddr) == -1)
		{
			printf("send error! pack_id:%d\n", back.id);
			continue;
		}

		if (strlen(pack.buf) < BUFSIZE - 1)
		{
			cout << "pack.buf:" << strlen(pack.buf) << endl;
			printf("receive finish\n");
			break;
		}
		memset(pack.buf, 0, sizeof(pack.buf));
		if (ret = UDP::serverRecvMessage(&pack, fd, p_remoteAddr) == -1)
		{
			printf("recv error! pack_id:%d\n", back.id + 1);
		}
		else
		{
			printf(" %d\n", pack.id);
			//pack.id++;
		}

		if (pack.id == (back.id + 1))
		{
			back.id++;
			fwrite(pack.buf, 1, BUFSIZE - 1, fp);
			printf("success recv pack id:%d\n", pack.id);
		}
		else
		{
			printf("failed recv pack id[%d]\n", pack.id + 1);
		}
	}
	fclose(fp);
	return 0;
}

DWORD WINAPI Server::SendFileThread(LPVOID lpParams)
{
	cout << "start send\n";
	struct sockaddr_in* p_remoteAddr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));

	FILE* fp = fopen(globalClient.sendFile, "rb+");
	if (fp == NULL)
	{
		printf("file open error");
		exit(1);
	}

	RBackInfo back;
	RPackInfo pack;
	int id = 1;

	if (UDP::rserverRecvMessage(&back, fd, p_remoteAddr) != -1) // ��ȡ�Է���ַ
	{
		printf("recv client_addr\n");
	}

	while (1)
	{
		//ʹ�ļ�fp��λ��ָ��ָ���ļ���ʼ
		rewind(fp);
		
		pack.id = id;
		memset(pack.buf, 0, sizeof(pack.buf)); //���


		//�ض�λ���ϵ��ļ�ָ�� �ɹ�����0
		//��һ������streamΪ�ļ�ָ��
		//�ڶ�������offsetΪƫ������������ʾ����ƫ�ƣ�������ʾ����ƫ��
		//����������origin�趨���ļ������￪ʼƫ��, ����ȡֵΪ��SEEK_CUR�� SEEK_END �� SEEK_SET
		//SEEK_SET�� �ļ���ͷ
		//SEEK_CUR�� ��ǰλ��
		//SEEK_END�� �ļ���β
		//����SEEK_SET, SEEK_CUR��SEEK_END������Ϊ0��1��2.
		if (fseek(fp, (pack.id - 1)*(BUFSIZE - 1), SEEK_SET) != 0)
		{
			printf("fseek error");
			fclose(fp);
			exit(1);
		}

		//buffer   �Ƕ�ȡ�����ݴ�ŵ��ڴ��ָ�루���������飬Ҳ�������¿��ٵĿռ䣬buffer����һ��������
		//size     ��ÿ�ζ�ȡ���ֽ���
		//count    �Ƕ�ȡ����
		//stream   ��Ҫ��ȡ���ļ���ָ��
		int fret = fread(pack.buf, 1, BUFSIZE - 1, fp);
		if (fret == 0)
		{
			printf("send finish\n");
			break;
		}

		pack.buf[fret] = '\0'; // BUFSIZE ��β

		//cout << "buf:" << pack.buf << endl;

		if (UDP::rserverSendMessage(&pack, fd, p_remoteAddr) == -1)
		{
			printf("send error pack id:%d\n", pack.id);
			continue;
		}
		if (UDP::rserverRecvMessage(&back, fd, p_remoteAddr) == -1)
		{
			printf("recv error pack id:%d\n", pack.id);
		}
		if (pack.id == back.id)
		{
			printf("\nsuccess pack id:%d\n", pack.id);
			id++;
		}
		memset(pack.buf, 0, BUFSIZE);
	}
	fclose(fp);

	return 0;
}

unsigned Server::SyncRoomNumber(unsigned number) // ͬ�������
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
				if (!IsDigital(recvMes.messageBuffer)) // ��������
				{
					SendMes("your input error, please reinput room number:", number);	
				}
				else
				{
					roomNum = (unsigned)atoi(recvMes.messageBuffer);
					if (FindRoom(roomNum)) // ���ַ����
					{
						SendMes("room exist, enter room y/n or change?", number);
						status = ExistR;
					}
					else  // δ���ַ����
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
					else // ������
					{
						SendMes("please input room number:", number);
						status = InputRB;
					}
				}
				else if (strcmp(recvMes.messageBuffer, "change") == 0) // �޸ķ���ţ����ƣ�
				{
					SendMes("please input new room number:", number);
					status = ChangeRN;
				}
				else // ��������
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

						if (CreateRoom(roomNum)) // �����ɹ�
						{
							if (EnterRoom(roomNum, number)) // ����ɹ�
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
						else // �����Ѵ���
						{
							char str[128];
							sprintf(str, "room_%d has been exist,enter room y/n? ", roomNum);
							SendMes(str, number);
							status = ExistR;
						}
					}
					else // ������
					{
						SendMes("please input room number:", number);
						status = InputRB;
					}
				}
				else // ��������
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
	char sendBuffer[128];

	sprintf(sendBuffer, "%s", message);

	int sendByte = send(clientSocket[number - 1], sendBuffer, sizeof(sendBuffer), 0);//���ط������ݵ��ܺ�
	if (sendByte < 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		//cout << "send message to client_" << number << " successed:" << message << endl;
	}
}

void Server::TxtTranspond(ClientParams *clientInfo, CMessage* message) // ת������
{
	for (vector<Room>::iterator iter1 = rooms.begin(); iter1 != rooms.end();iter1++)
	{ // ����ƥ���û�
		if (iter1->roomNumber == clientInfo->roomId)
		{
			for (vector<unsigned>::iterator iter = iter1->mateList.begin(); iter != iter1->mateList.end(); iter++)
			{
				char sendBuffer[1024];

				(void)time(&now);
				timer = ctime(&now);

				sprintf(sendBuffer, "time:%suser_%d: %s\n", timer, clientInfo->userId, message->messageBuffer);

				if (message->pType == PUBLIC_MESSAGE && *iter != clientInfo->userId) // ��������
				{
					int sendByte = send(clientSocket[*iter - 1], sendBuffer, sizeof(sendBuffer), 0);//���ط������ݵ��ܺ�
					if (sendByte < 0)
					{
						cout << "send faild" << endl;
					}
					else
					{
						//cout << "transpond message to client_" << *iter << " successed: " << sendBuffer;
					}
				}
				else if (message->pType == PRIVATE_MESSAGE && *iter == message->sendObjId) // ��������
				{
					if (message->sendObjId == clientInfo->userId)
					{
						char sendBuffer[30];
						sprintf(sendBuffer, "forbid send to yourself");
						int sendByte = send(clientSocket[clientInfo->userId - 1], sendBuffer, sizeof(sendBuffer), 0);//���ط������ݵ��ܺ�
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
						int sendByte = send(clientSocket[message->sendObjId - 1], sendBuffer, sizeof(sendBuffer), 0);//���ط������ݵ��ܺ�
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

void Server::delServerMate(ClientParams params) // �ӷ�������Ա�б��Ƴ�
{
	for (vector<unsigned>::iterator iter = mateIds.begin(); iter != mateIds.end(); ++iter){
		if (*iter == params.userId)
		{
			mateIds.erase(iter);
			break;
		}
	}
}

void Server::ReStart() // ����������
{

}

void Server::Close() // �رշ�����
{
	closesocket(severSocket);
	WSACleanup();
}


// ROOM
bool Server::FindRoomMate(unsigned roomNum, unsigned number) // ���ҳ�Ա
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

void Server::delRoomMate(ClientParams params) // �ӷ����Ա�б��Ƴ�
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

bool Server::EnterRoom(unsigned roomNum, unsigned number) // ���뷿��
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

bool Server::CreateRoom(unsigned number) // ��������
{
	Room room(number);
	rooms.push_back(room);
	cout << "create room_" << number << endl;
	return true;
}

bool Server::FindRoom(unsigned number) // ���ҷ���
{
	for (vector<Room>::iterator iter = rooms.begin(); iter != rooms.end(); ++iter){
		if (iter->roomNumber == number){
			return true;
		}
	}
	return false;
}

bool Server::CloseRoom(unsigned number) // �رշ���
{
	return true;
}

bool Server::ChangeRoomNumber(unsigned oldNum, unsigned newNum, unsigned number) // �޸ķ����
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
bool Server::IsDigital(char str[]) // ʶ������
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

int Server::FuzzyMatch(const char* str) // ģ��ƥ��
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

	if (-1 != message.sendObjId)
	{
		int start = s.find("-/");

		message.mesLength = s.length() + 1 - start + 2;

		string newStr = s.substr(start + 2, message.mesLength);

		strncpy(message.messageBuffer, newStr.c_str(), newStr.length() + 1);

		message.pType = PRIVATE_MESSAGE;
	}
	else
	{
		message.mesLength = s.length() + 1;
		strncpy(message.messageBuffer, s.c_str(), s.length() + 1);
		message.pType = PUBLIC_MESSAGE;
	}

	WIN32_FIND_DATAA FindFileData;

	FindFirstFileA(s.c_str(), &FindFileData);

	if (s.find(":/") >= 0 && s.find(":/") < 10000 || s.find(":\\") >= 0 && s.find(":\\") < 10000)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			message.mType = FOLDER_MESSAGE;
		else
			message.mType = FILE_MESSAGE;
	}
	else
	{
		message.mType = TXT_MESSAGE;
	}

	return message;
}

