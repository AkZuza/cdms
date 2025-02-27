#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "cdms/cdms.h"
#include <pthread.h>


int running = 0;
pthread_mutex_t active, print;

void* read_thread(void* pvrecfd)
{
	char buf[128];
	size_t len = sizeof(buf);
	int recfd = *(int*)pvrecfd;

	while(1)
	{
		pthread_mutex_lock(&active);
		if(running == 0)
			return NULL;
		pthread_mutex_unlock(&active);
		
		int rec = recieve_message(recfd, buf, &len);
		if(rec > 128)
			continue;
		if(rec > 0)
		{
			pthread_mutex_lock(&print);
			printf("Recieved: %s\n", buf);
			pthread_mutex_unlock(&print);
		}
	}

	return NULL;
}

int main(int argc, char** argv)
{
	ChatSession session;
	session.user_socket = -1;
	session.rec_socket = -1;
	
	char buf[512] = {};
	int len = sizeof(buf);

	pthread_mutex_init(&active, NULL);
	pthread_mutex_init(&print, NULL);
	
	pthread_t reader_thread;

	while(1)
	{
		printf("\n1.Create chat\n2.Join chat\n3.Quit\n");
		int choice = 0;
		printf("Enter choice: ");
		scanf("%d", &choice);
		getchar();

		switch(choice)
		{
			case 1:
			{
				printf("\nEnter IP Address to start chat on: ");
				char address[16] = {};
				scanf("%s", address);
				uint16_t port = 0;
				printf("Enter port number [0-9999]: ");
				scanf("%hu", &port);
					
				create_chat(&session, address, port);
				if(session.user_socket == -1)
				{
					printf("Failed to create chat! Enter another IP Address\n");
					break;
				}

				printf("Successfuly created chat! Waiting for user to join...\n");
				wait_for_recipient_to_join(&session);
				
				printf("Someone joined!\n");
				running = 1;
				pthread_create(&reader_thread, NULL, read_thread, (void*)&session.rec_socket);
				
				while(1)
				{
					printf("\n1.Send message\n2.Close chat\n");
					printf("Enter choice: ");
					scanf("%d", &choice);
					getchar();

					if(choice == 2)
					{
						pthread_mutex_lock(&active);
						running = 0;
						pthread_mutex_unlock(&active);
						break;
					}
					else if(choice == 1)
					{
						printf("Enter message: ");
						fgets(buf, len, stdin);
						send_message(session.rec_socket, buf, strlen(buf));
						pthread_mutex_lock(&print);
						printf("Sent: %s\n", buf);
						pthread_mutex_unlock(&print);
					}	
				}

				pthread_join(reader_thread, NULL);
				printf("Chat closed!\n");
				close_chat(&session);
			break;
			}

			case 2:
			{

				printf("\nEnter IP Address to start chat on: ");
				char address[16] = {};
				scanf("%s", address);
				uint16_t port = 0;
				printf("Enter port number [0-9999]: ");
				scanf("%hu", &port);
					
				join_chat(&session, address, port);
				if(session.user_socket == -1)
				{
					printf("Failed to join chat! Enter valid IP Address\n");
					break;
				}

				running = 1;
				pthread_create(&reader_thread, NULL, read_thread, (void*)&session.user_socket);
				
				while(1)
				{
					printf("\n1.Send message\n2.Close chat\n");
					printf("Enter choice: ");
					scanf("%d", &choice);
					getchar();

					if(choice == 2)
					{
						pthread_mutex_lock(&active);
						running = 0;
						pthread_mutex_unlock(&active);
						break;
					}
					else if(choice == 1)
					{
						printf("Enter message: ");
						fgets(buf, len, stdin);
						send_message(session.user_socket, buf, strlen(buf));
						pthread_mutex_lock(&print);
						printf("Sent: %s\n", buf);
						pthread_mutex_unlock(&print);
					}	
				}

				pthread_join(reader_thread, NULL);
				printf("Chat closed!\n");
				close_chat(&session);

			break;
			}

			case 3:
			{
				close_chat(&session);
				return 0;
			}
		}
	}

	close_chat(&session);
	return 0;
}
