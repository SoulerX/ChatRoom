#define BUFSIZE 1024

enum CommunicationType{
	TCP_COMMUNICATION = 1, // TCP
	UDP_COMMUNICATION = 2, // UDP
};

enum PublicType{
	PUBLIC_MESSAGE = 1, // 公开消息
	PRIVATE_MESSAGE = 2, // 私有消息
};

enum MessageType{
	TXT_MESSAGE = 1, // 文字消息
	FILE_MESSAGE = 2, // 文件消息
	FOLDER_MESSAGE = 3, // 文件夹消息
};

typedef struct CMessage{

	PublicType pType = PUBLIC_MESSAGE; // 默认公开消息

	MessageType mType = TXT_MESSAGE; // 默认文字消息

	int sendObjId; // 发送对象id

	int mesLength; // 消息长度

	char messageBuffer[64 * BUFSIZE]; // 消息内容

}CMessage;

typedef struct FMessage{
	bool finish; // 本次发送是否完成 
	unsigned length; // 发送长度
	char sendMessage[2000]; // 发送内容
}FMessage;