#include "Client.h"

Client::Client() // 默认TCP初始化
{
	Init();
	cout << "loading version successed" << endl;
	cout << "\t\t\t\t ChatRoom 多人聊天程序 \n";
	cout << "\t\t\t *************【Client】************* \n";
	(void)time(&now);
	timer = ctime(&now);
	cout << "\t\t\t\t" << timer << endl;
	cout << "-----------------------------------------------------------------------------------------------" << endl;
}

Client::~Client()
{
	closesocket(tcpSocket);
	WSACleanup();

	Close();
}

void Client::Init() // 初始化客户端
{
	rf = false;

	udpSocket = udp.udpSocketInit();
	serAddr = udp.udpAddrConfig();

	tcpSocket = tcp.tcpSocketInit();
}

void Client::Connect()
{
	if (tcp.TcpConnect(tcpSocket)) // tcp连接成功
	{
		CreateThread(NULL, 0, &receThread, 0, 0, NULL); // 开启接收线程
	}

	char str[1024];

	while (1)
	{
		gets_s(str); // 获取输入

		if (strcmp(str, "")) // 非空
		{
			CMessage sendMessage = TypeRecognition(str); // 识别输入消息类型
			
			CreateThread(NULL, 0, &sendThread, &sendMessage, 0, NULL); // 发送线程
		}
		else
		{
			cout << "invalid input\n"; // 输入非法
		}
	}
}

void Client::ReConnect()
{

}

void Client::Close()
{

}

DWORD WINAPI Client::sendThread(LPVOID lpParam)
{
	CMessage* temp = (CMessage*)lpParam;

	CMessage message;

	message.mType = temp->mType;
	message.pType = temp->pType;
	message.mesLength = temp->mesLength;
	strcpy(message.messageBuffer, temp->messageBuffer);
	message.sendObjId = temp->sendObjId;

	int sendByte = send(tcpSocket, (char*)&message, sizeof(CMessage), 0);
	if (sendByte <= 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		if ((strcmp(message.messageBuffer, "accept") == 0) && (rf == true))
		{
			rf = false;
			CreateThread(NULL, 0, &RecvFile, 0, 0, NULL);
		}
	}
	
	return 0;
}

DWORD WINAPI Client::receThread(LPVOID lpParam)
{
	char receBuff[1024];

	while (1)
	{
		int recvByte = recv(tcpSocket, receBuff, sizeof(receBuff), 0);

		if (recvByte >0)
		{
			cout << receBuff << endl;

			string s = receBuff;

			int start = s.find_last_of(":");

			int end = s.find("accept or refuse?");

			if ( end >= 0 && end < 10000)
			{
				string temp = s.substr(start + 2, end - start - 4);
				fileName.push_back(temp);
				rf = true;
			}


			int start2 = s.find("accept1");

			if (start2>=0 && start2 < 10000)
			{
				CreateThread(NULL, 0, &SendFile, lpParam, 0, NULL);
			}
			
		}
		else
		{
			cout << "receive end" << endl;
			int reRecvByte = recv(tcpSocket, receBuff, sizeof(receBuff), 0);
			if (reRecvByte <= 0)
			{
				cout << "break server" << endl;
			}
			break;
		}
	}
	closesocket(tcpSocket);
	return 0;
}

DWORD WINAPI Client::SendFile(LPVOID lpParam)
{
	FILE* fp = fopen(fileName.back().c_str(), "rb+");
	if (fp == NULL)
	{
		printf("file open error");
		exit(1);
	}

	fseek(fp, 0, SEEK_END);
	long end = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	PackInfo pack;
	BackInfo back;

	int id = 1;

	while (1)
	{
		pack.id = id;
		pack.fin = false;

		if (fseek(fp, (pack.id - 1)*(BUFSIZE - 1), SEEK_SET) != 0)
		{
			printf("fseek error");
			fclose(fp);
			exit(1);
		}

		int res = fread(pack.buf, 1, BUFSIZE - 1, fp);
		pack.buf[res] = '\0';
		if (res == 0)
		{
			pack.fin = true;
			char temp[50];
			strcpy(pack.buf, _itoa(strlen(pack.buf), temp, 50));

			while (UDP::clientSendMessage(&pack, udpSocket, &serAddr) == -1)
			{
				cout << "send pack id:" << id << " faild\n";
			}	
			cout << "send break\n";
			break;
		}

		if (UDP::clientSendMessage(&pack, udpSocket, &serAddr) == -1)
		{
			cout << "send pack id:" << id << " faild\n";
			continue;
		}

		if (UDP::clientRecvMessage(&back, udpSocket) == -1)
		{
			cout << "recv back id:" << id << " faild\n";
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

DWORD WINAPI Client::RecvFile(LPVOID lpParams)
{
	cout << "start recv\n";

	printf("waiting for message...\n");

	RPackInfo pack;
	pack.id = 0;

	RBackInfo back;
	back.id = 1;

	if (UDP::rclientSendMessage(&back, udpSocket, &serAddr) != -1) // 先发送地址信息
	{
		printf("send addr\n");
	}


	int ret = 0;

	if ((ret = UDP::rclientRecvMessage(&pack, udpSocket)) == -1)
	{
		printf("the first package recv error");
		exit(1);
	}
	else
	{
		printf(" %d\n", pack.id);
		printf("success recv pack id:%d\n", pack.id);
	}

	FILE* fp = fopen(fileName.back().c_str(), "wb+");
	cout << "name:" << fileName.back().c_str() << endl;

	fwrite(pack.buf, 1, BUFSIZE - 1, fp);

	while (1)
	{

		if (UDP::rclientSendMessage(&back, udpSocket, &serAddr) == -1)
		{
			printf("send error! pack_id:%d\n", back.id);
			continue;
		}

		if (pack.fin) //长度不足 BUFSIZE - 1
		{
			printf("receive finish\n");
			break;
		}

		memset(pack.buf, 0, sizeof(pack.buf)); // 清空

		if (ret = UDP::rclientRecvMessage(&pack, udpSocket) == -1)
		{
			printf("recv error! pack_id:%d\n", back.id + 1);
		}
		else
		{
			printf(" %d\n", pack.id);
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

void Client::SendMes(char* message)
{
	char sendBuffer[128];

	sprintf(sendBuffer, "%s", message);

	int sendByte = send(tcpSocket, sendBuffer, sizeof(sendBuffer), 0);//返回发送数据的总和
	if (sendByte < 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		cout << "----"<<sendBuffer << endl;
	}
}


// TOOL
bool Client::IsDigital(char str[]) // 识别数字
{
	while (*str){
		if ((*str < '0' || *str > '9') && *str != '\n')
		{
			return false;
		}
		*str++;
	}
	return true;
}

int Client::FuzzyMatch(const char* str) // 模糊匹配
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

CMessage Client::TypeRecognition(string s)
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
	}

	WIN32_FIND_DATAA FindFileData;

	FindFirstFileA(message.messageBuffer, &FindFileData);
	
	if (newStr.size()<=0)
	{
		newStr = message.messageBuffer;
	}

	if (newStr.find(":\\") >= 0 && newStr.find(":\\") < 10000)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			message.mType = FOLDER_MESSAGE;
		}
		else
		{
			fileName.push_back(newStr);
			message.mType = FILE_MESSAGE;
		}
	}
	else
	{
		message.mType = TXT_MESSAGE;
	}

	return message;
}

