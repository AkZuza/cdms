#include "cdms.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int check_for_message(int rec_socket)
{
	return 0;
}

void send_message(int fd, char * buf, size_t len)
{
	int written = write(fd, buf, len);
	if(written == -1)
	{
		printf("Failed to write message\n");
	}
}

int recieve_message(int fd, char * buf, size_t * len)
{
	size_t recieved = recv(fd, buf, *len, MSG_DONTWAIT);
	return recieved;
}

void create_chat(ChatSession* session, char* ip_address, uint16_t port)
{
	int userfd = socket(AF_INET, SOCK_STREAM, 0);
	if(userfd == -1)
	{
		printf("Failed to create chat\n");
		return;
	}

	session->user_socket = userfd;

	struct sockaddr_in address;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	if(!inet_aton(ip_address, &address.sin_addr))
	{
		printf("Failed to decipher the IP Address\n");
		return;
	}

	// bind and set it to listening for incoming connections
	if(bind(userfd, (struct sockaddr*)&address, sizeof(address)) == -1)
	{
		printf("Failed to bind to %s\n", ip_address);
		return;
	}
	listen(userfd, 0);
}

void close_chat(ChatSession* session)
{
	close(session->user_socket);
	close(session->rec_socket);
}


void join_chat(ChatSession* session, char* ip_address, uint16_t port)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	session->user_socket = sockfd;

	struct sockaddr_in address;
	address.sin_port = htons(port);
	address.sin_family = AF_INET;
	if(!inet_aton(ip_address, &address.sin_addr))
	{
		printf("Failed to decipher the IP Address\n");
		return;
	}

	if(connect(sockfd, (struct sockaddr*)&address, sizeof(address)))
	{
		printf("Failed to connect to %s\n", ip_address);
		return;
	}
}

void wait_for_recipient_to_join(ChatSession* session)
{
	struct sockaddr address;
	socklen_t len = sizeof(address);
	int recfd = accept(session->user_socket, &address, &len);
	if(recfd == -1)
	{
		printf("Failed to accept incoming connection\n");
		return;
	}
	session->rec_socket = recfd;
}
