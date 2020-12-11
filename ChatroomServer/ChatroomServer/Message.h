
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
	FLODER_MESSAGE = 3, // �ļ�����Ϣ
};

typedef struct CMessage{

	PublicType pType = PUBLIC_MESSAGE; // Ĭ�Ϲ�����Ϣ

	MessageType mType = TXT_MESSAGE; // Ĭ��������Ϣ

	int sendObjId; // ���Ͷ���id

	int mesLength; // ��Ϣ����

	char messageBuffer[1024]; // ��Ϣ����

}CMessage;