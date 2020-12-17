#include "stdafx.h"
#include "Message.h"


class UDP
{

public:

	SOCKET udpSocketInit();
	sockaddr_in udpAddrConfig();

	static int clientSendMessage(PackInfo* pack, int cliSocket, struct sockaddr_in* p_serAddr);
	static int clientRecvMessage(BackInfo* back, int cliSocket);

	static int rclientSendMessage(RBackInfo* back, int cliSocket, struct sockaddr_in* p_serAddr);
	static int rclientRecvMessage(RPackInfo* pack, int cliSocket);
};