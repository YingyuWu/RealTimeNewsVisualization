#pragma once
#include <stdio.h>
#include <winsock2.h>
#include <string>
#include <sstream>
#include <iostream>
#include <windows.h>

#include "base64.h"
#include "sha1.h"

#pragma comment (lib, "ws2_32.lib")

#define BUFFER_SIZE 16384
#define DLLEXPORT __declspec(dllexport)
#define  OP_READ 1
#define  OP_WRITE 2
#define	 OP_ACCEPT 3

typedef struct _PER_HANDLE_DATA
{
	SOCKET sock;
	sockaddr_in si;
	bool handshaken;
}PER_HANDLE_DATA,*PPER_HANDLE_DATA;

typedef struct _PER_IO_DATA
{
	OVERLAPPED overlapped;
	WSABUF wsabuf;
	char buffer[BUFFER_SIZE];
	int operationType;
}PER_IO_DATA,*PPER_IO_DATA;

class DLLEXPORT websocketserver
{
public:
	websocketserver(int port, std::string ip);
	~websocketserver(void);

	std::string getMessage();
	void sendMessage(std::string newMessage);

	HANDLE m_hCompletion;

private:
	static DWORD WINAPI StaticThreadStartForMainThread(void *Param)
	{
		websocketserver* _this = (websocketserver *)Param;
		return _this->MainThread();

	}
	static DWORD WINAPI StaticThreadStartForServerThread(void *Param)
	{	
		websocketserver* _this = (websocketserver *)Param;
		return _this->ServerThread(_this->m_hCompletion);
	}

	DWORD MainThread(void);
	DWORD ServerThread(LPVOID lParam);
	DWORD ServerThread_win7(LPVOID lParam);

	std::string handshakeResponseMessageCreater(char *buffer);
	char* decodeTheMessageFromClient(char *buffer, int bytesRecv);
	std::string encodeTheMessageToClient(std::string rawString);
private:
	int m_port;
	std::string m_ip;
	SOCKET m_listen_socket;
	SOCKET m_accepted_sockets;
	sockaddr_in m_address_socket;

	std::string m_message;

	int m_current_received_message;
	int m_current_total_message;
	char *m_current_masks;
	bool m_is_in_stream;
};
