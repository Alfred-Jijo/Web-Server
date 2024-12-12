#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>

#include <stdio.h>
#include <string.h>
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

#pragma comment(lib, "Ws2_32.lib")

int main(void) {
	WSADATA wsadata;
	int err = WSAStartup(MAKEWORD(2, 2), &wsadata);
	if (err != 0) {
		printf("WSAStartup failed with error code: %d\n", err);
		WSACleanup();
		return EXIT_FAILURE;
	}
	
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) {
		printf("socket function failed with error code: %d\n", WSAGetLastError());
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = 0;
	addr.sin_port = htons(8080);

	err = bind(sock, &addr, sizeof(addr));
	if (err == SOCKET_ERROR) {
		printf("bind failed with error %u\n", WSAGetLastError());
		closesocket(sock);
		WSACleanup();
		return EXIT_FAILURE;
	}

	listen(sock, 10);
	SOCKET client = accept(sock, 0, 0);
	if (client == INVALID_SOCKET) {
		printf("accept failed with error: %d\n", WSAGetLastError());
		closesocket(client);
		WSACleanup();
		return 1;
	}

	char request[256] = {0};
	int buflen = recv(client, request, 256, 0);
	if (buflen != 256) {
		printf("recv failed with error: %d\n", WSAGetLastError());
	}

	if(memcmp(request, "GET / ", 6) == 0) {
		FILE *file;
		fopen_s(&file, "index.html", "r");
		printf("HTML equested\n");
		char buffer[256] = {0};
		fread(buffer, 1, 256, file);
		send(client, buffer, 256, 0);
		printf("HTML sent");
	} 

	char c;
	scanf("%c", &c);
	closesocket(sock);
	WSACleanup();
	return EXIT_SUCCESS;
}

