#ifndef _CDMS_H_
#define _CDMS_H_

#include <stdint.h>
#include <stdlib.h>

struct ChatSession {
	int user_socket;
	int rec_socket;
};
typedef struct ChatSession ChatSession;

// blocking, waits for recipient to join
// returns the recipient socket fd
void wait_for_recipient_to_join(ChatSession* session);

void send_message(int rec_socket, char* buf, size_t buf_len);
int check_for_message(int rec_socket);
int recieve_message(int rec_socket, char* buf, size_t* buf_len);

void close_chat(ChatSession* session);

void create_chat(ChatSession* session, char* ip_address, uint16_t port);
void join_chat(ChatSession* session, char* ip_address, uint16_t port);

#endif // _CDMS_H_
