
#define BUFSIZE 1024

enum CommunicationType{
	TCP_COMMUNICATION = 1, // TCP
	UDP_COMMUNICATION = 2, // UDP
};

enum PublicType{
	PUBLIC_MESSAGE = 1, // ������Ϣ
	PRIVATE_MESSAGE = 2, // ˽����Ϣ
};

enum MessageType{
	TXT_MESSAGE = 1, // ������Ϣ
	FILE_MESSAGE = 2, // �ļ���Ϣ
	FOLDER_MESSAGE = 3, // �ļ�����Ϣ
};

typedef struct CMessage{
	
	PublicType pType = PUBLIC_MESSAGE; // Ĭ�Ϲ�����Ϣ

	MessageType mType = TXT_MESSAGE; // Ĭ��������Ϣ

	int sendObjId; // ���Ͷ���id

	int mesLength; // ��Ϣ����

	char messageBuffer[64 * BUFSIZE]; // ��Ϣ����

}CMessage;

typedef struct FMessage{
	bool finish;
	unsigned length;
	char sendMessage[2000];
}FMessage;

typedef struct ClientInfo{
	char sendObj[10];
	char sendFile[1024];
}ClientInfo;

typedef struct PackInfo
{
	int id;
	bool fin;
	char buf[60 * BUFSIZE];
}PackInfo;

typedef struct BackInfo
{
	int id;
	int status;
}BackInfo;

typedef struct RPackInfo
{
	int id;
	bool fin;
	char buf[60*BUFSIZE];
}RPackInfo;

typedef struct RBackInfo
{
	int id;
	int status;
}RBackInfo;
