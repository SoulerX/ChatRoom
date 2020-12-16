#include "Client.h"
#include "UDP.h"


Client::Client(CommunicationType type) // 默认TCP初始化
{
	WORD require = MAKEWORD(2, 2);

	if (WSAStartup(require, &wsaData) != 0)
	{
		cout << "loading winsock faild" << endl;
		return;
	}
	cout << "loading winsock successed" << endl;

	if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
	{
		cout << "request version faild" << endl;
		return;
	}
	cout << "loading version successed" << endl;

	if (Init(type))
	{
		cout << "init finish\n\n\n" << endl;
		cout << "\t\t\t\t ChatRoom 多人聊天程序 \n";
		cout << "\t\t\t *************【Client】************* \n";
		(void)time(&now);
		timer = ctime(&now);
		cout << "\t\t\t\t" << timer << endl;
		cout << "-----------------------------------------------------------------------------------------------" << endl;
	}
	else
	{
		cout << "init faild" << endl;
	}
}

Client::~Client()
{
	closesocket(clientSocket);
	WSACleanup();

	Close();
}

bool Client::Init(CommunicationType type) // 初始化客户端
{
	rf = false;

	UDP udp;

	fd = udp.clientSocketInit();

	if (type == TCP_COMMUNICATION)
	{
		clientSocket = socket(AF_INET, SOCK_STREAM, 0);

		if (clientSocket == -1)
		{
			cout << "create socket faild" << endl;
			return false;
		}
		cout << "create socket successed" << endl;

		sockaddr_in addr;
		addr.sin_family = AF_INET;
		//addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//10.230.136.55 
		addr.sin_addr.S_un.S_addr = inet_addr("192.168.1.87");
		addr.sin_port = htons(25000);

		int b;
		b = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));//
		if (b == -1)
		{
			cout << WSAGetLastError() << endl;
			cout << "connect faild" << endl;
			return false;
		}
		cout << "server has been successfully connected, you can send message" << endl;

		return true;
	}
	else
	{
		// TODO: udp
		return false;
	}
}

void Client::Connect()
{
	char str[1024];

	CreateThread(NULL, 0, &receThread, 0, 0, NULL);

	while (1)
	{
		gets_s(str);

		if (strcmp(str, "")) // 非空
		{
			CMessage sendMessage = TypeRecognition(str);
			
			CreateThread(NULL, 0, &sendThread, &sendMessage, 0, NULL);
		}
		else
		{
			cout << "invalid input\n";
		}
	}
}

void Client::ReConnect()
{
	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");//10.230.136.55 
	addr.sin_port = htons(25000);

	int b;
	b = connect(clientSocket, (sockaddr*)&addr, sizeof(addr));//
	if (b == -1)
	{
		cout << WSAGetLastError() << endl;
		cout << "connect faild" << endl;
		return;
	}
	cout << "server has been successfully connected, you can send message" << endl;
	return;
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

	int sendByte = send(clientSocket, (char*)&message, sizeof(CMessage), 0);
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
		int recvByte = recv(clientSocket, receBuff, sizeof(receBuff), 0);

		if (recvByte >0)
		{
			cout << receBuff << endl;

			string s = receBuff;

			int start = s.find_last_of(":");

			int end = s.find("accept or refuse?");

			if ( end >= 0 && end < 10000)
			{
				string temp = s.substr(start + 2, end - start - 4);
				cout << "temp:" << temp << endl;
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
			int reRecvByte = recv(clientSocket, receBuff, sizeof(receBuff), 0);
			if (reRecvByte <= 0)
			{
				cout << "break server" << endl;
			}
			break;
		}
	}
	closesocket(clientSocket);
	return 0;
}

DWORD WINAPI Client::SendFile(LPVOID lpParam)
{
	int port = 8888;
	char addr[] = "127.0.0.1";

	//规定对于客户端的目的地址的一些参数
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(addr);

	FILE* fp = fopen(fileName[0].c_str(), "rb+");
	if (fp == NULL)
	{
		printf("file open error");
		exit(1);
	}

	BackInfo back;

	int id = 1;

	while (1)
	{
		rewind(fp);
		PackInfo pack;
		pack.id = id;

		if (fseek(fp, (pack.id - 1)*(BUFSIZE - 1), SEEK_SET) != 0)
		{
			printf("fseek error");
			fclose(fp);
			exit(1);
		}

		int fret = fread(pack.buf, 1, BUFSIZE - 1, fp);
		if (fret == 0)
		{
			printf("send finish\n");
			break;
		}
		pack.buf[fret] = '\0';
		if (UDP::clientSendMessage(&pack, fd, &serAddr) == -1)
		{
			printf("send error pack id:%d\n", pack.id);
			continue;
		}
		if (UDP::clientRecvMessage(&back, fd) == -1)
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

DWORD WINAPI Client::RecvFile(LPVOID lpParams)
{
	int port = 8888;
	char addr[] = "127.0.0.1";

	//规定对于客户端的目的地址的一些参数
	struct sockaddr_in serAddr;
	serAddr.sin_family = AF_INET;
	serAddr.sin_port = htons(port);
	serAddr.sin_addr.S_un.S_addr = inet_addr(addr);

	cout << "start recv\n";

	printf("waiting for message...\n");

	RPackInfo pack = { 0, 0, { 0 } };
	pack.id = 0;

	RBackInfo back;
	back.id = 1;

	if (UDP::rclientSendMessage(&back, fd, &serAddr) != -1) // 先发送地址信息
	{
		printf("send addr\n");
	}


	int ret = 0;

	if ((ret = UDP::rclientRecvMessage(&pack, fd)) == -1)
	{
		printf("the first package recv error");
		exit(1);
	}
	else
	{
		printf(" %d\n", pack.id);
		printf("success recv pack id:%d\n", pack.id);
	}

	FILE* fp = fopen(fileName[0].c_str(), "wb+");
	fwrite(pack.buf, 1, BUFSIZE - 1, fp);

	while (1)
	{

		if (UDP::rclientSendMessage(&back, fd, &serAddr) == -1)
		{
			printf("send error! pack_id:%d\n", back.id);
			continue;
		}

		if (strlen(pack.buf) < BUFSIZE - 1) //长度不足 BUFSIZE - 1
		{
			printf("receive finish\n");
			break;
		}

		memset(pack.buf, 0, sizeof(pack.buf)); // 清空

		if (ret = UDP::rclientRecvMessage(&pack, fd) == -1)
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

	int sendByte = send(clientSocket, sendBuffer, sizeof(sendBuffer), 0);//返回发送数据的总和
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

