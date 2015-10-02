#include "ServerProcessing.h"

void main()
{

	ServerProcessing myServer(2014, "127.0.0.1");

	//myServer.run(1200); // interval bases on second
	myServer.run(3600*24);

	system("pause");
}