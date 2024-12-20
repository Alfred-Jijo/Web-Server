#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <winsock2.h>

#ifndef UNICODE
#define UNICODE
#endif //UNICODE

#include <stdio.h>
#include <string.h>
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define forever while(1)

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 8192

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
	WSACleanup();
	return EXIT_FAILURE;
    }

    struct sockaddr_in addr;
    ZeroMemory(&addr, sizeof(addr));  // Initialize the structure
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(8080);

    err = bind(sock, (struct sockaddr*)&addr, sizeof(addr));
    if (err == SOCKET_ERROR) {
	printf("bind failed with error %u\n", WSAGetLastError());
	closesocket(sock);
	WSACleanup();
	return EXIT_FAILURE;
    }

    if (listen(sock, 10) == SOCKET_ERROR) {
	printf("listen failed with error: %d\n", WSAGetLastError());
	closesocket(sock);
	WSACleanup();
	return EXIT_FAILURE;
    }

    printf("Server listening on port 8080...\n");

    do {  // Server loop
	SOCKET client = accept(sock, NULL, NULL);
	if (client == INVALID_SOCKET) {
	    printf("accept failed with error: %d\n", WSAGetLastError());
	    continue;  // Continue listening for next connection
	}

	char request[BUFFER_SIZE] = {0};
	int buflen = recv(client, request, BUFFER_SIZE - 1, 0);
	if (buflen < 0) {
	    printf("recv failed with error: %d\n", WSAGetLastError());
	    closesocket(client);
	    continue;
	}

	if(memcmp(request, "GET / ", 6) == 0) {
	    FILE *file;
	    if (fopen_s(&file, "index.html", "rb") != 0) {
		printf("Failed to open index.html\n");
		const char* error_response =
		    "HTTP/1.1 404 Not Found\r\n"
		    "Content-Type: text/plain\r\n"
		    "Connection: close\r\n\r\n"
		    "404 - File not found";
		send(client, error_response, strlen(error_response), 0);
		closesocket(client);
		continue;
	    }

	    // Get file size
	    fseek(file, 0, SEEK_END);
	    long file_size = ftell(file);
	    fseek(file, 0, SEEK_SET);

	    // Allocate buffer for the whole file
	    char* file_buffer = (char*)malloc(file_size);
	    if (!file_buffer) {
		fclose(file);
		closesocket(client);
		continue;
	    }

	    // Read file
	    size_t bytes_read = fread(file_buffer, 1, file_size, file);
	    fclose(file);

	    // Prepare and send HTTP header
	    char header[512];
	    sprintf_s(header, sizeof(header),
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: %ld\r\n"
		"Connection: close\r\n"
		"\r\n", file_size);

	    send(client, header, strlen(header), 0);
	    send(client, file_buffer, file_size, 0);

	    free(file_buffer);
	    printf("HTML sent successfully\n");
	} else {
	    // Handle unknown request
	    const char* not_found =
		"HTTP/1.1 404 Not Found\r\n"
		"Content-Type: text/plain\r\n"
		"Connection: close\r\n\r\n"
		"404 - Not Found";
	    send(client, not_found, strlen(not_found), 0);
	}

	closesocket(client);
    } forever;

    closesocket(sock);
    WSACleanup();
    return EXIT_SUCCESS;
}