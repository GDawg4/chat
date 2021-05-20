#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <signal.h>
#include "new.pb-c.h"
#define MAX_MSG_SIZE 1024
// #include "amessage.pb-c.h"

#define MAX_CLIENTS 100
#define BUFFER_SZ 2048 * 24

static _Atomic unsigned int cli_count = 0;
static int uid = 10;

/* Client structure */
typedef struct
{
	struct sockaddr_in address;
	int sockfd;
	int uid;
	char status[32];
	char name[32];
} client_t;

client_t *clients[MAX_CLIENTS];

pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void str_overwrite_stdout()
{
	printf("\r%s", "> ");
	fflush(stdout);
}

// char* get_ip(struct sockaddr_in addr)
// {

// 	char ip[BUFFER_SZ]= {};
// 	sprintf(ip,"%d.%d.%d.%d",
// 		   addr.sin_addr.s_addr & 0xff,
// 		   (addr.sin_addr.s_addr & 0xff00) >> 8,
// 		   (addr.sin_addr.s_addr & 0xff0000) >> 16,
// 		   (addr.sin_addr.s_addr & 0xff000000) >> 24);
// 	printf("IP FUNCTION",ip);
// 	return ip;
// }

/* Add clients to queue */
void queue_add(client_t *cl)
{	
	strcpy(cl->status, "activo");
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (!clients[i])
		{
			clients[i] = cl;
			
			break;
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Remove clients to queue */
void queue_remove(int uid)
{
	pthread_mutex_lock(&clients_mutex);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid == uid)
			{
				clients[i] = NULL;
				break;
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

/* Send message to all clients except sender */
// void broadcast_message(char *s, int uid)
// {
// 	pthread_mutex_lock(&clients_mutex);

// 	for (int i = 0; i < MAX_CLIENTS; ++i)
// 	{
// 		if (clients[i])
// 		{
// 			if (clients[i]->uid != uid)
// 			{
// 				if (write(clients[i]->sockfd, s, strlen(s)) < 0)
// 				{
// 					perror("ERROR: write to descriptor failed");
// 					break;
// 				}
// 			}
// 		}
// 	}

// 	pthread_mutex_unlock(&clients_mutex);
// }

/* Send message to all clients except sender */
void broadcast_message(char *msg_string, client_t *client_sender)
{
	pthread_mutex_lock(&clients_mutex);
	// printf("Chat General %s -> %s\n", msg.sender, msg.message);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid != client_sender->uid)
			{

				Chat__ServerResponse srv_res = CHAT__SERVER_RESPONSE__INIT;
				void *buf; // Buffer to store serialized data
				unsigned len;
				srv_res.option = 4;
				Chat__MessageCommunication msg = CHAT__MESSAGE_COMMUNICATION__INIT; // AMessage
				msg.message = msg_string;
				msg.recipient = "everyone";
				msg.sender = client_sender->name;
				srv_res.messagecommunication = &msg;
				len = chat__server_response__get_packed_size(&srv_res);
				buf = malloc(len);
				chat__server_response__pack(&srv_res, buf);

				if (send(clients[i]->sockfd, buf, len, 0) < 0)
				{
					sendFailureServerResponse("Error sending broadcast message.", client_sender, 0);
					break;
				}

				free(buf);
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
	// sendSuccessServerResponse("Message sent succesfully", client_sender,0);
}

void sendSuccessServerResponse(char *succces_message, client_t *client_sender, int option)
{
	pthread_mutex_lock(&clients_mutex);

	Chat__ServerResponse srv_res = CHAT__SERVER_RESPONSE__INIT;
	void *buf;	  // Buffer to store serialized data
	unsigned len; // Length of serialized data

	srv_res.code = 200;
	srv_res.servermessage = succces_message;
	if (option != 0)
	{
		srv_res.option = option;
	}
	len = chat__server_response__get_packed_size(&srv_res);
	buf = malloc(len);
	chat__server_response__pack(&srv_res, buf);
	send(client_sender->sockfd, buf, len, 0);
	pthread_mutex_unlock(&clients_mutex);
}

void sendFailureServerResponse(char *failure_message, client_t *client_sender, int option)
{
	pthread_mutex_lock(&clients_mutex);

	Chat__ServerResponse srv_res = CHAT__SERVER_RESPONSE__INIT;
	void *buf;	  // Buffer to store serialized data
	unsigned len; // Length of serialized data

	srv_res.code = 500;
	srv_res.servermessage = failure_message;
	if (option != 0)
	{
		srv_res.option = option;
	}
	len = chat__server_response__get_packed_size(&srv_res);
	buf = malloc(len);
	chat__server_response__pack(&srv_res, buf);
	send(client_sender->sockfd, buf, len, 0);
	pthread_mutex_unlock(&clients_mutex);
}

/* Change User Status*/
// void broadcast_message("Hola a todos",cli)
// {
// 	printf("FUCK");
// 	// printf("status ad %s",status);
// 	// pthread_mutex_lock(&clients_mutex);
	
// 	// for (int i = 0; i < MAX_CLIENTS; ++i)
// 	// {
// 	// 	if (clients[i])
// 	// 	{
// 	// 		if (strcmp(clients[i]->name, username) == 0)
// 	// 		{
				
				
// 	// 			strcpy(clients[i]->status, status);
// 	// 			sendSuccessServerResponse("Status changed succesfully.",client,3);
// 	// 			//send message to everyone that someone changed status
// 	// 			char buff_out2[BUFFER_SZ];
// 	// 			sprintf(buff_out2, "%s has changed to status %s\n", username, status);
// 	// 			printf("Chat General %s has changed to status %s\n", username, status);
// 	// 			broadcast_message(buff_out2, client);
				

// 	// 		}
// 	// 	}
// 	// }
// 	// pthread_mutex_unlock(&clients_mutex);
// 	// sendFailureServerResponse("Trying to change status of user that doesnt exit.", client, 3);

// }

/* Send private message*/
void send_private_message(char *msg_string, client_t *client_sender, char *receiverName)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (strcmp(clients[i]->name, receiverName) == 0)
			{
				Chat__ServerResponse srv_res = CHAT__SERVER_RESPONSE__INIT;
				void *buf; // Buffer to store serialized data
				unsigned len;
				srv_res.option = 4;
				Chat__MessageCommunication msg = CHAT__MESSAGE_COMMUNICATION__INIT; // AMessage
				msg.message = msg_string;
				msg.recipient = receiverName;
				msg.sender = client_sender->name;
				srv_res.messagecommunication = &msg;
				len = chat__server_response__get_packed_size(&srv_res);
				buf = malloc(len);
				chat__server_response__pack(&srv_res, buf);

				if (send(clients[i]->sockfd, buf, len, 0) < 0)
				{
					sendFailureServerResponse("Error sending broadcast message. User is no longer connected", client_sender, 0);
					pthread_mutex_unlock(&clients_mutex);
					return;
				}
				else
				{
					printf("Chat Privado %s hacia %s -> %s\n", client_sender->name, receiverName, msg_string);
					pthread_mutex_unlock(&clients_mutex);
					return;
				}

				free(buf);
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
	sendFailureServerResponse("Trying to send private message to user that doesnt exit.", client_sender, 0);
}

/* Return response a user*/
void return_response_to_sender(char *s, int uid)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid == uid)
			{
				if (write(clients[i]->sockfd, s, strlen(s)) < 0)
				{
					perror("ERROR: write to descriptor failed");
					break;
				}
			}
		}
	}

	pthread_mutex_unlock(&clients_mutex);
}

int check_is_name_available_in_clients(char *name, int uid)
{
	pthread_mutex_lock(&clients_mutex);
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			if (clients[i]->uid != uid)
			{
				if (strcmp(clients[i]->name, name) == 0)
				{
					pthread_mutex_unlock(&clients_mutex);
					return 0;
				}
			}
		}
	}
	pthread_mutex_unlock(&clients_mutex);
	return 1;
}

int check_is_ip_available_in_clients(int uid, struct sockaddr_in addr)
{
	pthread_mutex_lock(&clients_mutex);

	char ip1[BUFFER_SZ];
	sprintf(ip1, "%d.%d.%d.%d",
			addr.sin_addr.s_addr & 0xff,
			(addr.sin_addr.s_addr & 0xff00) >> 8,
			(addr.sin_addr.s_addr & 0xff0000) >> 16,
			(addr.sin_addr.s_addr & 0xff000000) >> 24);

	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i])
		{
			char ip2[BUFFER_SZ];
			sprintf(ip2, "%d.%d.%d.%d",
					clients[i]->address.sin_addr.s_addr & 0xff,
					(clients[i]->address.sin_addr.s_addr & 0xff00) >> 8,
					(clients[i]->address.sin_addr.s_addr & 0xff0000) >> 16,
					(clients[i]->address.sin_addr.s_addr & 0xff000000) >> 24);
			if (clients[i]->uid != uid && strcmp(ip1, ip2) == 0)
			{

				pthread_mutex_unlock(&clients_mutex);
				return 0;
			}
			bzero(ip2, BUFFER_SZ);
		}
	}
	bzero(ip1, BUFFER_SZ);
	pthread_mutex_unlock(&clients_mutex);
	return 1;
}

/* Handle all communication with the client */
void *handle_client(void *arg)
{

	char buff_out[BUFFER_SZ];
	char name[32];
	int leave_flag = 0;

	cli_count++;
	client_t *cli = (client_t *)arg;
	// int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
	if (recv(cli->sockfd, buff_out, BUFFER_SZ, 0) <= 0)
	{
		sendFailureServerResponse("Error sending message in client.\n", cli, 1);
		leave_flag = 1;
	}

	Chat__ClientPetition *cli_ptn_register;
	Chat__UserRegistration *user;
	cli_ptn_register = chat__client_petition__unpack(NULL, strlen(buff_out), buff_out);
	int optionRegister = (cli_ptn_register->option);
	user = cli_ptn_register->registration;
	if (optionRegister != 1)
	{
		sendFailureServerResponse("User registration was expected.\n", cli, 1);
		leave_flag = 1;
	}
	if (strlen(user->username) < 2 || strlen(user->username) >= 32 - 1)
	{
		sendFailureServerResponse("Name must be between 2 and 32 characters.\n", cli, 1);
		leave_flag = 1;
	}
	else
	{

		if (check_is_name_available_in_clients(user->username, cli->uid) == 0)
		{
			leave_flag = 1;
			sendFailureServerResponse("Client name already exists. Use a different name\n", cli, 1);
		}
		if (check_is_ip_available_in_clients(cli->uid, cli->address) == 0)
		{
			leave_flag = 1;
			sendFailureServerResponse("Client IP already exists. Unable to connect\n", cli, 1);
		}
		if (leave_flag == 0)
		{
			strcpy(cli->name, user->username);
			sprintf(buff_out, "%s has joined\n", cli->name);
			printf("%s", buff_out);
			sendSuccessServerResponse("***** WELCOME TO THE C CHAT ***** \n", cli, 1);
			broadcast_message(buff_out, cli);
		}
	}

	bzero(buff_out, BUFFER_SZ);

	while (1)
	{
		if (leave_flag)
		{
			break;
		}

		int receive = recv(cli->sockfd, buff_out, BUFFER_SZ, 0);
		if (receive > 0)
		{
			if (strlen(buff_out) > 0)
			{

				//str_trim_lf(buff_out, strlen(buff_out));

				if (strcmp(buff_out, "hola") == 0)
				{
					printf("Hey");

					return_response_to_sender("HI", cli->uid);
					// printf("%s -> %s\n", buff_out, cli->name);
				}
				else
				{

					Chat__ClientPetition *cli_ptn;
					Chat__MessageCommunication *msg;
					// Read packed message from standard-input.
					// Unpack the message using protobuf-c.

					cli_ptn = chat__client_petition__unpack(NULL, strlen(buff_out), buff_out);
					int option = (cli_ptn->option);

					switch (option)
					{
					case 1:
						// broadcast_message();
						break;
					case 2:
						
					
						break;
					case 3:
					
					
						
						printf("Client s %s\n", cli_ptn->change->status);
						printf("Client u %s\n", cli_ptn->change->username);
						
						break;
					case 4:

						msg = cli_ptn->messagecommunication;
						if (msg == NULL)
						{
							fprintf(stderr, "Error message received was null\n");
							break;
						}

						// printf("\n");
						if (strcmp(msg->recipient, "everyone") == 0)
						{

							char buff_out2[BUFFER_SZ];
							sprintf(buff_out2, "%s\n", msg->message);
							printf("Chat General %s -> %s\n", msg->sender, msg->message);
							broadcast_message(buff_out2, cli);
						}
						else
						{
							char buff_out2[BUFFER_SZ];
							sprintf(buff_out2, "%s\n", msg->message);

							send_private_message(buff_out2, cli, msg->recipient);
						}
						// Free the unpacked message
						chat__message_communication__free_unpacked(msg, NULL);
						break;
					case 5:
						printf("5\n");
						break;
					case 6:
						printf("6\n");
						break;
					case 7:
						printf("Gracias por usar el chat!\n");

						break;
					default:
						printf("Wrong Choice. Enter again\n");
						break;
					}
				}
			}
		}
		else if (receive == 0 || strcmp(buff_out, "exit") == 0)
		{
			sprintf(buff_out, "%s has left\n", cli->name);
			printf("%s", buff_out);
			broadcast_message(buff_out, cli);
			leave_flag = 1;
		}
		else
		{
			printf("ERROR: -1\n");
			leave_flag = 1;
		}

		bzero(buff_out, BUFFER_SZ);
	}

	/* Delete client from queue and yield thread */
	close(cli->sockfd);
	queue_remove(cli->uid);
	free(cli);
	cli_count--;
	pthread_detach(pthread_self());

	return NULL;
}

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		printf("Usage: %s <port>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char *ip = "127.0.0.1";
	int port = atoi(argv[1]);
	int option = 1;
	int listenfd = 0, connfd = 0;
	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	pthread_t tid;

	/* Socket settings */
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(port);

	/* Ignore pipe signals */
	signal(SIGPIPE, SIG_IGN);

	if (setsockopt(listenfd, SOL_SOCKET, (SO_REUSEPORT | SO_REUSEADDR), (char *)&option, sizeof(option)) < 0)
	{
		perror("ERROR: setsockopt failed");
		return EXIT_FAILURE;
	}

	/* Bind */
	if (bind(listenfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		perror("ERROR: Socket binding failed");
		return EXIT_FAILURE;
	}

	/* Listen */
	if (listen(listenfd, 10) < 0)
	{
		perror("ERROR: Socket listening failed");
		return EXIT_FAILURE;
	}

	printf("=== WELCOME TO THE CHATROOM ===\n");

	while (1)
	{
		socklen_t clilen = sizeof(cli_addr);
		connfd = accept(listenfd, (struct sockaddr *)&cli_addr, &clilen);

		/* Check if max clients is reached */
		if ((cli_count + 1) == MAX_CLIENTS)
		{
			printf("Max clients reached. Rejected: ");

			printf(":%d\n", cli_addr.sin_port);
			close(connfd);
			continue;
		}

		/* Client settings */

		client_t *cli = (client_t *)malloc(sizeof(client_t));
		cli->address = cli_addr;
		cli->sockfd = connfd;
		cli->uid = uid++;

		/* Add client to the queue and fork thread */
		queue_add(cli);
		pthread_create(&tid, NULL, &handle_client, (void *)cli);

		/* Reduce CPU usage */
		sleep(1);
	}

	return EXIT_SUCCESS;
}