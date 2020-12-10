
enum CommunicationType{
	TCP_COMMUNICATION = 1, // TCP
	UDP_COMMUNICATION = 2, // UDP
};

enum PublicType{
	PUBLIC_MESSAGE = 0x01, // ������Ϣ
	PRIVATE_MESSAGE = 0x02, // ˽����Ϣ
};

enum MessageType{
	TXT_MESSAGE = 0x11, // ������Ϣ
	FILE_MESSAGE = 0x12, // �ļ���Ϣ
};

typedef struct CMessage{

	MessageType mType = TXT_MESSAGE; // Ĭ��������Ϣ

	PublicType pType = PUBLIC_MESSAGE; // Ĭ�Ϲ�����Ϣ

	unsigned sendObjId; // ���Ͷ���id

	unsigned roomId; // ���� id

	unsigned userId; // �û� id

	unsigned mesLength; // ��Ϣ����

	char messageBuffer[128]; // ��Ϣ����

}CMessage;