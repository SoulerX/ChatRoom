#include "stdafx.h"

#define BUFSIZE 512

typedef struct PackInfo
{
	int id;
	char buf[BUFSIZE];
}PackInfo;

typedef struct BackInfo
{
	int id;
	int status;
}BackInfo;

typedef struct RPackInfo
{
	int id;
	char buf[BUFSIZE];
}RPackInfo;

typedef struct RBackInfo
{
	int id;
	int status;
}RBackInfo;

class UDP
{

public:

	int clientSocketInit();

	static int clientSendMessage(PackInfo* pack, int cliSocket, struct sockaddr_in* p_serAddr);
	static int clientRecvMessage(BackInfo* back, int cliSocket);

	static int rclientSendMessage(RBackInfo* back, int cliSocket, struct sockaddr_in* p_serAddr);
	static int rclientRecvMessage(RPackInfo* pack, int cliSocket);
};