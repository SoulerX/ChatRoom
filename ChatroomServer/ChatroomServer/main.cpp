#include "stdafx.h"
#include "Server.h"

int main(void)
{
	//system("mode con lines=60 cols=54 ");

	system("color 17");

	Server myServer;

	myServer.Start();

	system("pause");

	return 0;
}