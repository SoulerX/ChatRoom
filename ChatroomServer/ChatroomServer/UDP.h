#include "stdafx.h"

#define	BUFSIZE 512

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
	unsigned serverSocketInit();

	static int serverRecvMessage(PackInfo* pack, int serSocket, struct sockaddr_in* p_remoteAddr);
	static int serverSendMessage(BackInfo* back, int serSocket, struct sockaddr_in* p_remoteAddr);
	static int rserverRecvMessage(RBackInfo* back, int serSocket, struct sockaddr_in* p_remoteAddr);
	static int rserverSendMessage(RPackInfo* pack, int serSocket, struct sockaddr_in* p_remoteAddr);
};