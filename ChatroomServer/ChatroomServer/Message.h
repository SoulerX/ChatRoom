
enum CommunicationType{
	TCP_COMMUNICATION = 1, // TCP
	UDP_COMMUNICATION = 2, // UDP
};

enum PublicType{
	PUBLIC_MESSAGE = 0x01, // 公开消息
	PRIVATE_MESSAGE = 0x02, // 私有消息
};

enum MessageType{
	TXT_MESSAGE = 0x11, // 文字消息
	FILE_MESSAGE = 0x12, // 文件消息
};

typedef struct CMessage{

	MessageType mType = TXT_MESSAGE; // 默认文字消息

	PublicType pType = PUBLIC_MESSAGE; // 默认公开消息

	unsigned sendObjId; // 发送对象id

	unsigned roomId; // 房间 id

	unsigned userId; // 用户 id

	unsigned mesLength; // 消息长度

	char messageBuffer[128]; // 消息内容

}CMessage;