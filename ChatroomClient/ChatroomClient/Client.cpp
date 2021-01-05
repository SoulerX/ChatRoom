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

DWORD WINAPI Client::sendThread(LPVOID lpParam)
{
	CMessage* temp = (CMessage*)lpParam;

	// 防止不同主机地址指针出错
	CMessage message;
	memset(message.messageBuffer, '\0', 64 * BUFSIZE);
	message.mType = temp->mType;
	message.pType = temp->pType;
	message.mesLength = temp->mesLength;
	strcpy(message.messageBuffer, temp->messageBuffer);
	message.sendObjId = temp->sendObjId;

	if (message.mType == FILE_MESSAGE) // 发送文件 截断
	{
		CreateThread(NULL, 0, &SendFile, 0, 0, NULL);
		return 0;
	}

	int sendByte = send(tcpSocket, (char*)&message, sizeof(CMessage), 0);
	if (sendByte <= 0)
	{
		cout << "send faild" << endl;
	}
	else
	{
		if ((strcmp(message.messageBuffer, "accept") == 0) && (rf == false))
		{
			rf = true;
			//CreateThread(NULL, 0, &RecvFile, 0, 0, NULL);
		}
	}
	
	return 0;
}

DWORD WINAPI Client::receThread(LPVOID lpParam)
{
	CMessage receBuff;

	while (1)
	{
		memset(receBuff.messageBuffer, '\0', BUFSIZE);

		int recvByte = recv(tcpSocket, (char*)&receBuff, sizeof(CMessage), 0);
		if (recvByte >0)
		{
			if (receBuff.mType == TXT_MESSAGE)
			{
				cout << receBuff.messageBuffer << endl;

				

				string s = receBuff.messageBuffer;

				int start = s.find_last_of("\\");

				int end = s.find(",");

				if (end >= 0 && end < 10000 && start > 0 && start < 10000 )
				{
					string temp = s.substr(start + 1, end - start - 1);
					fileName.push_back(temp);
					cout << "filename:" << fileName.back() << endl;
					rf = false;
				}
			}
			else if (receBuff.mType == FILE_MESSAGE)
			{
				if (rf)
				{
					cout << "start recv\n";
					cout << "waiting for message...\n";

					fp = fopen(fileName.back().c_str(), "wb+");
					cout << "name:" << fileName.back().c_str() << endl;
					rf = false;
					starttime = clock();
				}
				
				int ret = 0;

				ret = fwrite(receBuff.messageBuffer, 1, receBuff.mesLength, fp);

				if (ret > 0)
				{
					cout << "recv size " << receBuff.mesLength << endl;
					totalsize += receBuff.mesLength;
					cout << ++record << " total size " << totalsize << endl;

					if (ret < 64 * BUFSIZE)
					{
						cout << "recv finish" << endl;
						endtime = clock();
						double delta = (double)(endtime - starttime) / CLOCKS_PER_SEC;
						printf("time = %f s  speed = %f mb/s\n", delta, totalsize / delta / 1024 / 1024);
						
						totalsize = 0;
						record = 0;

						fclose(fp);	
					}
				}
				else
				{
					cout << "error" << endl;
				}
			}
			else
			{

			}
		}
		else
		{
			cout << "break server" << endl;
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
		printf("file open error\n");
		exit(1);
	}
	printf("file open success\n");

	int size = _filelength(_fileno(fp));
	int rsize = size;
	cout << "size:" << size << endl;

	CMessage message;
	memset(message.messageBuffer, '\0', 64 * BUFSIZE);
	message.mType = TXT_MESSAGE;
	sprintf(message.messageBuffer, "file %s %d", fileName.back().c_str(), size);
	cout << message.messageBuffer << endl;

	int ret = 0;
	if ((ret = send(tcpSocket, (char*)&message, sizeof(CMessage), 0)) > 0)
	{
		cout << "send fileinfo success" << endl;
	}

	clock_t start, end;
	start = clock();

	int i = 0;
	while (size > 0) {
		memset(message.messageBuffer, '\0', 64 * BUFSIZE);

		message.mType = FILE_MESSAGE;

		int temp = (size < 64 * BUFSIZE ? size : 64 * BUFSIZE);

		message.mesLength = temp;

		fread(message.messageBuffer, 1, temp, fp);

		int ret = 0;
		if ((ret = send(tcpSocket, (char*)&message, sizeof(CMessage), 0)) == -1)
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
			}
			else
			{
				cout << ++i << " residue size " << size << endl;
			}
		}
		//Sleep(10);
	}

	end = clock();

	double delta = (double)(end - start) / CLOCKS_PER_SEC;

	printf("time = %f s  speed = %f mb/s\n", delta, rsize/delta/1024/1024);

	fclose(fp);
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

