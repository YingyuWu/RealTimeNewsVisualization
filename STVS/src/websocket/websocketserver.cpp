#include "websocketserver.h"

#define my_ip "127.0.0.1" //"127.0.0.1"

websocketserver::websocketserver(int port, std::string ip)
{
	m_ip = ip;
	m_port = port;
	m_message = "";
	m_accepted_sockets = NULL;
	// Create Listen Thread
	CreateThread (NULL, 0, StaticThreadStartForMainThread, (void*)this, 0, 0);

	m_current_received_message = 0;
	m_current_total_message = 0;
	m_current_masks = new char [4];
	memset(m_current_masks, 0x00, 4*sizeof(char));
	m_is_in_stream = false;
	m_lock = false;

}

websocketserver::~websocketserver(void)
{
}

std::string websocketserver::getMessage()
{
	if (m_lock) {
		return "";
	}
	else {
		return m_message;
	}
}

void websocketserver::sendMessage(std::string rawnewMessage)
{
	while (m_is_in_stream){
		int waiting = 0;
	}
	std::string newMessage = encodeTheMessageToClient(rawnewMessage);

	if (send(m_accepted_sockets, newMessage.c_str(), newMessage.length(), 0) == SOCKET_ERROR)
	{
		std::cout << "websocketserver::sendMessage() failed." << std::endl;
	}
}

typedef struct _CLIENT : OVERLAPPED  
{  
    HANDLE          hPipe;  
    BOOL            fAwaitingAccept;  
    PVOID           Zero;  
    BOOL  Message;  
} CLIENT, *PCLIENT;

DWORD websocketserver::ServerThread(LPVOID lParam)
{
	HANDLE hCompletion = (HANDLE)lParam;
	DWORD dwTrans;
	PPER_HANDLE_DATA pPer_Handle_Data = NULL;
	PCLIENT test = NULL;
	PPER_IO_DATA pPer_IO_Data;

	while (true)
	{
		bool bOk = GetQueuedCompletionStatus(hCompletion,&dwTrans,(PULONG_PTR)&test,(LPOVERLAPPED*)&pPer_IO_Data,INFINITE);
		if(!bOk) // ERROR
		{
			std::cout << GetLastError() << std::endl;
			closesocket(m_accepted_sockets);

			delete pPer_Handle_Data;
			delete pPer_IO_Data;

			continue;
		}
		if(dwTrans==0 && (pPer_IO_Data->operationType==OP_READ || pPer_IO_Data->operationType==OP_WRITE)) // Client Close the Socket
		{
			closesocket(m_accepted_sockets);

			delete pPer_Handle_Data;

			delete pPer_IO_Data;
			continue;
		}

		if (1) // New Connection needs handshake
		{
			m_message = "";

			std::string responseMessage = handshakeResponseMessageCreater(pPer_IO_Data->buffer);
			char *responseBuffer = new char [responseMessage.length()];
			strcpy(responseBuffer, responseMessage.c_str());
			int response_size = responseMessage.length();

			send(m_accepted_sockets, responseBuffer, response_size, 0);
			//pPer_Handle_Data->handshaken = true;

			int bytesRecv = 0;
			char recvbuf[BUFFER_SIZE];

			while (true)
			{
				bytesRecv = recv(m_accepted_sockets, recvbuf, sizeof(recvbuf), 0);

				if (bytesRecv == 0 || bytesRecv == -1)
					break;

				if (!m_is_in_stream){

					m_current_received_message = bytesRecv;

					if ((unsigned char)recvbuf[0] == 0x88){

						char* closeMessage = new char[3];
						closeMessage[0]=0x88;
						closeMessage[1]=0x00;
						closeMessage[2]=0x00;
						send(m_accepted_sockets, closeMessage, 3, 0);
						break;
					}

					m_message = decodeTheMessageFromClient(recvbuf, bytesRecv);
				}else{

					m_current_received_message += bytesRecv;
					m_message += decodeTheMessageFromClient(recvbuf, bytesRecv);
				}

			}
		}

	}

	return 0;
}
DWORD websocketserver::ServerThread_win7(LPVOID lParam)
{
	HANDLE hCompletion = (HANDLE)lParam;
	DWORD dwTrans;
	PPER_HANDLE_DATA pPer_Handle_Data;
	PPER_IO_DATA pPer_IO_Data;

	while (true)
	{
		bool bOk = GetQueuedCompletionStatus(hCompletion,&dwTrans,(PULONG_PTR)&pPer_Handle_Data,(LPOVERLAPPED*)&pPer_IO_Data,INFINITE);
		if(!bOk) // ERROR
		{
			std::cout << GetLastError() << std::endl;
			closesocket(pPer_Handle_Data->sock);

			delete pPer_Handle_Data;
			delete pPer_IO_Data;

			continue;
		}
		if(dwTrans==0 && (pPer_IO_Data->operationType==OP_READ || pPer_IO_Data->operationType==OP_WRITE)) // Client Close the Socket
		{
			closesocket(pPer_Handle_Data->sock);

			delete pPer_Handle_Data;

			delete pPer_IO_Data;
			continue;
		}

		if (!pPer_Handle_Data->handshaken) // New Connection needs handshake
		{
			m_message = "";

			std::string responseMessage = handshakeResponseMessageCreater(pPer_IO_Data->buffer);
			char *responseBuffer = new char [responseMessage.length()];
			strcpy(responseBuffer, responseMessage.c_str());
			int response_size = responseMessage.length();

			send(pPer_Handle_Data->sock, responseBuffer, response_size, 0);
			pPer_Handle_Data->handshaken = true;

			int bytesRecv = 0;
			char recvbuf[BUFFER_SIZE];

			while (true)
			{
				bytesRecv = recv(pPer_Handle_Data->sock, recvbuf, sizeof(recvbuf), 0);

				if (bytesRecv == 0 || bytesRecv == -1)
					break;

				m_lock = true;
				if (!m_is_in_stream){

					m_current_received_message = bytesRecv;

					if ((unsigned char)recvbuf[0] == 0x88){

						char* closeMessage = new char[3];
						closeMessage[0]=0x88;
						closeMessage[1]=0x00;
						closeMessage[2]=0x00;
						send(m_accepted_sockets, closeMessage, 3, 0);
						break;
					}

					m_message = decodeTheMessageFromClient(recvbuf, bytesRecv);
				}else{

					m_current_received_message += bytesRecv;
					m_message += decodeTheMessageFromClient(recvbuf, bytesRecv);
				}
				m_lock = false;

			}
		}

	}

	return 0;
}

DWORD websocketserver::MainThread(void)
{
	WSADATA wsa;
	// Request Winsock version 2.2
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) 
	{
		std::cout << "Server: WSAStartup Failed.\n" << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;
	}

	// Create TCP socket object
	m_listen_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listen_socket == INVALID_SOCKET){
		std::cout << "Server: socket() failed.\n" << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;
	}

	// Set socket address
	m_address_socket.sin_family = AF_INET;
	m_address_socket.sin_port = htons(m_port);
	m_address_socket.sin_addr.s_addr = inet_addr(m_ip.c_str());

	// Create Server Thread
	m_hCompletion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	CreateThread(NULL, 0, StaticThreadStartForServerThread, (void *)this, 0, 0);

	//
	if (bind(m_listen_socket,(SOCKADDR *)&m_address_socket,sizeof(SOCKADDR)) == SOCKET_ERROR)
	{
		std::cout << "Server: bind() failed.\n" << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;
	}

	if (listen(m_listen_socket,2) == SOCKET_ERROR)
	{
		std::cout << "Server: listen() failed.\n" << WSAGetLastError() << "\n";
		WSACleanup();
		return -1;
	}

	while (true)
	{
		sockaddr_in remoteInfo;
		int remoteLen = sizeof(remoteInfo);
		SOCKET acceptSocket = accept(m_listen_socket, (sockaddr*)&remoteInfo, &remoteLen);

		if (acceptSocket == INVALID_SOCKET)
		{
			continue;
		}

		if (m_accepted_sockets != NULL)
		{
			closesocket(m_accepted_sockets);
		}

		m_accepted_sockets = acceptSocket;

		PPER_HANDLE_DATA pPerHandleData = new PER_HANDLE_DATA();
		pPerHandleData->sock = acceptSocket;
		pPerHandleData->handshaken = false;
		memcpy(&(pPerHandleData->si), &remoteInfo, sizeof(sockaddr));

		CreateIoCompletionPort((HANDLE)pPerHandleData->sock, m_hCompletion, (DWORD)pPerHandleData, 0);

		PPER_IO_DATA pPer_IO_Data = new PER_IO_DATA();
		pPer_IO_Data->operationType = OP_READ;
		pPer_IO_Data->wsabuf.buf=pPer_IO_Data->buffer;
		pPer_IO_Data->wsabuf.len=BUFFER_SIZE;
		DWORD dwRecv;
		DWORD dwFlags=0;

		memset(&pPer_IO_Data->overlapped,0,sizeof(pPer_IO_Data->overlapped));

		::WSARecv(pPerHandleData->sock,&(pPer_IO_Data->wsabuf),
			1,&dwRecv,&dwFlags,&(pPer_IO_Data->overlapped),NULL);

	}

	WSACleanup();
	return 0;
}

std::string websocketserver::handshakeResponseMessageCreater(char *buffer)
{
	std::string sec_websocket_key;
	std::string sec_websocket_accept;
	std::string line;
	std::string::size_type end;

	std::istringstream s(buffer);

	while (std::getline(s, line)){
		end = line.find(": ", 0);
		if (end != std::string::npos)
		{
			std::string key = line.substr(0, end);
			std::string val = line.substr(end+2);
			val.erase(val.end()-1);

			if (key == "Sec-WebSocket-Key")
			{
				sec_websocket_key = val;
				break;
			}
		}
	}

	sec_websocket_key += "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
	//258EAFA5-E914-47DA-95CA-C5AB0DC85B11

	SHA1        sha;
	unsigned int    message_digest[5];

	sha.Reset();
	sha << sec_websocket_key.c_str();

	sha.Result(message_digest);
	// convert sha1 hash bytes to network byte order because this sha1
	//  library works on ints rather than bytes
	for (int i = 0; i < 5; i++) {
		message_digest[i] = htonl(message_digest[i]);
	}

	sec_websocket_accept = base64_encode(
		reinterpret_cast<const unsigned char*>
		(message_digest),20
		);

	//		HTTP/1.1 101 Switching Protocols
	//		Connection: Upgrade
	//		Sec-WebSocket-Accept: ESr41TmXnufCWkD8MPvo58s7Vsg=
	//		Upgrade: websocket
	std::stringstream result;
	result << "HTTP/1.1 101 Switching Protocols\r\n";
	result << "Connection: Upgrade\r\n";
	result << "Sec-WebSocket-Accept: " << sec_websocket_accept << "\r\n";
	result << "Upgrade: websocket\r\n";
	result << "\r\n";

	return result.str();
}

char* websocketserver::decodeTheMessageFromClient(char *buffer, int bytesRecv)
{
	if (buffer == NULL)
		return "";

	if(!m_is_in_stream){

		char secondByte = buffer[1];
		int buffer_length = secondByte & 127;
		int indexFirstMask = 2;
		if (buffer_length == 126)            // if a special case, change indexFirstMask
			indexFirstMask = 4;
		else if (buffer_length == 127)       // ditto
			indexFirstMask = 10;

		switch (indexFirstMask)
		{
		case 2:
			m_current_total_message = buffer_length;
			m_current_received_message -= 6;
			break;
		case 4:
			unsigned * value_4;
			char temp_4[4];
			temp_4[0] = buffer[3];
			temp_4[1] = buffer[2];
			temp_4[2] = 0x00;
			temp_4[3] = 0x00;
			value_4 = (unsigned int *)&temp_4;
			m_current_total_message = *value_4;
			m_current_received_message -= 8;
			break;
		case 10:
			char temp_10[8];
			unsigned long long * value_10;
			temp_10[0] = buffer[9];
			temp_10[1] = buffer[8];
			temp_10[2] = buffer[7];
			temp_10[3] = buffer[6];
			temp_10[4] = buffer[5];
			temp_10[5] = buffer[4];
			temp_10[6] = buffer[3];
			temp_10[7] = buffer[2];
			value_10 = (unsigned long long *)&temp_10;
			m_current_total_message = *value_10;
			m_current_received_message -= 14;
			break;
		}

		if (m_current_total_message > m_current_received_message){
			m_is_in_stream = true;
		}
		else {
			m_current_received_message = m_current_total_message;
		}

		memcpy (m_current_masks, &buffer[indexFirstMask], 4);
		int indexFirstDataByte = indexFirstMask + 4;

		//int decodebuf_length = bytesRecv - indexFirstDataByte;
		int decodebuf_length = m_current_received_message;
		char *decodebuf = new char [decodebuf_length + 1];

		for (int k=indexFirstDataByte, l=0; l<decodebuf_length; k++, l++)
		{
			decodebuf[l] = buffer[k] ^ m_current_masks[l%4];
		}

		decodebuf[decodebuf_length] = '\0';

		return decodebuf;
	}
	else{
		if (m_current_total_message > m_current_received_message){
			m_is_in_stream = true;
		}else{
			m_current_received_message = 0;
			m_is_in_stream = false;
		}

		int decodebuf_length = bytesRecv;
		char *decodebuf = new char [decodebuf_length + 1];

		for (int l=0; l<decodebuf_length;l++)
		{
			decodebuf[l] = buffer[l] ^ m_current_masks[l%4];
		}

		decodebuf[decodebuf_length] = '\0';
		return decodebuf;
	}

}

std::string websocketserver::encodeTheMessageToClient(std::string rawString)
{
	if (rawString.length() == 0)
		return "";

	int indexStartRawData = -1;
	int send_message_length = rawString.length();

	char *sendbuf = NULL;

	if (send_message_length <= 125)
	{
		sendbuf = new char [send_message_length + 2 + 1];
		sendbuf[0] = 130; //0x82
		sendbuf[1] = send_message_length;
		indexStartRawData = 2;
	}
	else if (125 < send_message_length && send_message_length <= 65535)
	{
		sendbuf = new char [send_message_length + 4 + 1];
		sendbuf[0] = 130; //0x82
		sendbuf[1] = 126;
		sendbuf[2] = (send_message_length >> 8) & 255;
		sendbuf[3] = (send_message_length ) & 255;

		indexStartRawData = 4;
	}
	else
	{
		sendbuf = new char [send_message_length + 10 + 1];
		sendbuf[0] = 130; //0x82
		sendbuf[1] = 127;
		sendbuf[2] = 0x00; //(send_message_length >> 56) & 255;
		sendbuf[3] = 0x00; //(send_message_length >> 48) & 255;
		sendbuf[4] = 0x00; //(send_message_length >> 40) & 255;
		sendbuf[5] = 0x00; //(send_message_length >> 32) & 255;
		sendbuf[6] = (send_message_length >> 24) & 255;
		sendbuf[7] = (send_message_length >> 16) & 255;
		sendbuf[8] = (send_message_length >> 8) & 255;
		sendbuf[9] = (send_message_length ) & 255;

		indexStartRawData = 10;
	}

	memcpy (&sendbuf[indexStartRawData], rawString.c_str(), send_message_length);

	sendbuf[indexStartRawData+send_message_length] = '\0';
	std::string result = std::string(sendbuf, indexStartRawData+send_message_length);

	delete [] sendbuf;
	return result;
}

