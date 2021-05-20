#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <netdb.h>
#include "new.pb-c.h"
// #include "amessage.pb-c.h"
#define MAX_MSG_SIZE 1024
#define BUFFER_SZ 2048 * 24
#define LENGTH 2048

// Global variables
volatile sig_atomic_t flag = 0;
int sockfd = 0;
char name[32];
struct hostent *host;
void str_overwrite_stdout()
{
    printf("%s", "> ");
    fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
    int i;
    for (i = 0; i < length; i++)
    { // trim \n
        if (arr[i] == '\n')
        {
            arr[i] = '\0';
            break;
        }
    }
}

void catch_ctrl_c_and_exit(int sig)
{
    flag = 1;
}

void send_msg_handler()
{
    char message[LENGTH] = {};
    char buffer[LENGTH + 32] = {};

    while (1)
    {
        str_overwrite_stdout();
        fgets(message, LENGTH, stdin);
        str_trim_lf(message, LENGTH);

        if (strcmp(message, "exit") == 0)
        {
            break;
        }
        else
        {
            sprintf(buffer, "%s: %s\n", name, message);
            send(sockfd, buffer, strlen(buffer), 0);
        }

        bzero(message, LENGTH);
        bzero(buffer, LENGTH + 32);
    }
    catch_ctrl_c_and_exit(2);
}

void broadcast_message()
{
    char message[LENGTH] = {};
    char buffer[LENGTH + 32] = {};
    char temp;
    printf("Ingresa tu mensaje o 'exit' para volver al menú principal.\n");
    scanf("%c", &temp);
    str_overwrite_stdout();
    scanf("%[^\n]", &message);

    Chat__ClientPetition cli_ptn = CHAT__CLIENT_PETITION__INIT;
    Chat__MessageCommunication msg = CHAT__MESSAGE_COMMUNICATION__INIT; // AMessage
    void *buf;                                                          // Buffer to store serialized data
    unsigned len;                                                       // Length of serialized data
    // printf("%s\n", message);
    msg.message = message;
    msg.recipient = "everyone";
    msg.sender = name;

    cli_ptn.messagecommunication = &msg;
    cli_ptn.option = 4;

    len = chat__client_petition__get_packed_size(&cli_ptn);
    buf = malloc(len);
    chat__client_petition__pack(&cli_ptn, buf);

    if (strcmp(message, "exit") == 0)
    {
        return;
    }
    else
    {
        send(sockfd, buf, len, 0);
    }

    free(buf); // Free the allocated serialized buffer

    bzero(message, LENGTH);
    bzero(buffer, LENGTH + 32);
}

void private_message()
{
    char message[LENGTH] = {};
    char user_name[LENGTH] = {};
    char buffer[LENGTH + 32] = {};
    char temp;
    scanf("%c", &temp);
    printf("Ingresa el nombre del usuario a quien deseas enviarle el mensaje.\n");
    scanf("%[^\n]", &user_name);
    scanf("%c", &temp);
    printf("Ingresa tu mensaje o 'exit' para volver al menú principal.\n");
    str_overwrite_stdout();
    scanf("%[^\n]", &message);

    Chat__ClientPetition cli_ptn = CHAT__CLIENT_PETITION__INIT;
    Chat__MessageCommunication msg = CHAT__MESSAGE_COMMUNICATION__INIT; // AMessage
    void *buf;                                                          // Buffer to store serialized data
    unsigned len;                                                       // Length of serialized data
    // printf("%s\n", message);
    msg.message = message;
    msg.recipient = user_name;
    msg.sender = name;

    cli_ptn.messagecommunication = &msg;
    cli_ptn.option = 4;

    len = chat__client_petition__get_packed_size(&cli_ptn);
    buf = malloc(len);
    chat__client_petition__pack(&cli_ptn, buf);

    if (strcmp(message, "exit") == 0)
    {
        return;
    }
    else
    {
        send(sockfd, buf, len, 0);
    }

    free(buf); // Free the allocated serialized buffer

    bzero(message, LENGTH);
    bzero(buffer, LENGTH + 32);
}


void change_status()
{
    int choice_status;
    char *status;
    char buffer[LENGTH + 32] = {};
    char temp;
    scanf("%c", &temp);
    printf("Ingresa la opción para cambiar de status:\n");
    printf("1. activo\n");
    printf("2. inactivo\n");
    printf("3. ocupado\n");
    scanf("%d", &choice_status);

    switch (choice_status)
    {
    case 1:
        status = "activo";
        break;
    case 2:
        status = "inactivo";
        break;
    case 3:
        status = "ocupado";
        break;
    default:
        printf("La opción ingresada es icorrecta\n");
        return;
    }

    Chat__ClientPetition cli_ptn = CHAT__CLIENT_PETITION__INIT;
    Chat__ChangeStatus new_status = CHAT__CHANGE_STATUS__INIT; // AMessage
    void *buf;                                                          // Buffer to store serialized data
    unsigned len;              

    new_status.status = status;
    new_status.username = name;

    cli_ptn.option = 3;
    cli_ptn.change = &new_status;
    //printf("New Status %s\n",cli_ptn.change->status);
    //printf("New Name %s\n",cli_ptn.change->username);
    len = chat__client_petition__get_packed_size(&cli_ptn);
    buf = malloc(len);
    chat__client_petition__pack(&cli_ptn, buf);
    send(sockfd, buf, len, 0);


    free(buf); // Free the allocated serialized buffer

    bzero(buffer, LENGTH + 32);
}

void user_information()
{
    char user_name[LENGTH] = {};
    char buffer[LENGTH + 32] = {};
    char temp;
    scanf("%c", &temp);
    printf("Ingresa el nombre del usuario de quien deseas ver la información.\n");
    scanf("%[^\n]", &user_name);

    Chat__ClientPetition cli_ptn = CHAT__CLIENT_PETITION__INIT;
    Chat__UserRequest user_request = CHAT__USER_REQUEST__INIT; // AMessage
    void *buf;                                                          // Buffer to store serialized data
    unsigned len;                                                       // Length of serialized data
    // printf("%s\n", message);
    user_request.user = user_name;

    cli_ptn.user = &user_request;
    cli_ptn.option = 5;

    len = chat__client_petition__get_packed_size(&cli_ptn);
    buf = malloc(len);
    chat__client_petition__pack(&cli_ptn, buf);

    if (strcmp(message, "exit") == 0)
    {
        return;
    }
    else
    {
        send(sockfd, buf, len, 0);
    }

    free(buf); // Free the allocated serialized buffer

    bzero(message, LENGTH);
    bzero(buffer, LENGTH + 32);
}

void client_menu_handler()
{

    while (flag == 0)
    {
        int choice;

        printf("\nMenu:\n");
        printf("1. Chatear contodos los usuarios (broadcasting).\n");
        printf("2. Enviar y recibir mensajes directos, privados, aparte del chat general.\n");
        printf("3. Cambiar de status.\n");
        printf("4. Listar los usuarios conectados al sistema de chat.\n");
        printf("5. Desplegar información de un usuario en particular.\n");
        printf("6. Ayuda.\n");
        printf("7. Salir.\n");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            broadcast_message();
            break;
        case 2:
            private_message();
            break;
        case 3:
            change_status();
            break;
        case 4:
            user_information();
            break;
        case 5:
            printf("5\n");
            break;
        case 6:
            printf("6\n");
            break;
        case 7:
            printf("Gracias por usar el chat!\n");
            catch_ctrl_c_and_exit(2);
            break;
        default:
            printf("Wrong Choice. Enter again\n");
            break;
        }
    }
    catch_ctrl_c_and_exit(2);
}

void recv_msg_handler()
{
    char buff_out[BUFFER_SZ];
    while (1)
    {

        int receive = recv(sockfd, buff_out, BUFFER_SZ, 0);
        if (receive > 0)
        {
            Chat__ServerResponse *server_res;
            Chat__MessageCommunication *msg;
            Chat__UserInfo *user_info;

            server_res = chat__server_response__unpack(NULL, strlen(buff_out), buff_out);

            //Get Response Code
            int code = server_res->code;
            int option = (server_res->option);
            if (option!=0)
            {
                switch (option)
                {
                //User Register Response
                case 1:
                    printf("2\n");
                    break;
                //Connected User Response
                case 2:
                    printf("2\n");
                    break;
                //Change Status Response
                case 3:
                    if (code == 200){
                        //Print Success Message
                        printf("%s\n", server_res->servermessage);
                    }
                    else if (code == 500)
                    {
                        //Print Error Message
                        printf("%s\n", server_res->servermessage);
                    }
                    break;
                //Messages Response
                case 4:
                    msg = server_res->messagecommunication;

                    if (strcmp(msg->recipient, "everyone") == 0)
                    {
                        printf("Chat General enviado por %s -> %s\n", msg->sender, msg->message);
                    }
                    else
                    {
                        printf("Chat Privado enviado por %s -> %s\n", msg->sender, msg->message);
                    }
                    break;
                //User Information Response
                case 5:
                    user_info = server_res->userinforesponse;
                     if (code == 200)
                    {
                        printf("Usuario: %s \nStatus: %s\nIP: %s\n", user_info->username, user_info->status, user_info->ip);
                    }
                    else if (code == 500)
                    {
                        //Print Error Message
                        printf("%s\n", server_res->servermessage);
                    }
                    break;
                default:
                    break;
                }
            }
            else if (code == 200 && option==0){
                //Print Error Message
                printf("%s\n", server_res->servermessage);
                
            }
            else if (code == 500 && option==0)
            {
                //Print Error Message
                printf("%s\n", server_res->servermessage);
            }
        }
        else if (receive == 0)
        {
            break;
        }
        else
        {
            // -1
        }
        bzero(buff_out, BUFFER_SZ);
        // memset(message, 0, sizeof(message));
    }
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    host = gethostbyname(argv[1]);
    if (!host)
    {
        fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
        return -4;
    }
    int port = atoi(argv[2]);

    signal(SIGINT, catch_ctrl_c_and_exit);

    printf("Please enter your name: ");
    fgets(name, 32, stdin);
    str_trim_lf(name, strlen(name));

    if (strlen(name) > 32 || strlen(name) < 2)
    {
        printf("Name must be less than 30 and more than 2 characters.\n");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;

    /* Socket settings */
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    server_addr.sin_family = AF_INET;

    server_addr.sin_port = htons(port);

    memcpy(&server_addr.sin_addr, host->h_addr_list[0], host->h_length);
    // Connect to Server
    int err = connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (err == -1)
    {
        printf("ERROR: connect\n");
        return EXIT_FAILURE;
    }

    char *ip_client;
    ip_client = "3.142.76.26";
    

     //Create User Registration
    Chat__ClientPetition cli_ptn_register = CHAT__CLIENT_PETITION__INIT;
    Chat__UserRegistration user = CHAT__USER_REGISTRATION__INIT; 
    void *buf;                                                          
    unsigned len;              
    // Chat__UserRegistration user = &user;
    // strcpy(user->username, name);
    // strcpy(user->ip, ipBuffer);
    user.username = name;
    user.ip = ip_client;
 
    cli_ptn_register.option = 1;
    cli_ptn_register.registration = &user;
    
    len = chat__client_petition__get_packed_size(&cli_ptn_register);
    buf = malloc(len);
    chat__client_petition__pack(&cli_ptn_register, buf);

    //Send User Registation
    send(sockfd, buf, len, 0);

    char buff_out[BUFFER_SZ];
    int receive = recv(sockfd, buff_out, BUFFER_SZ, 0);
    printf("Ya paso\n");
    if (receive > 0)
    {
        Chat__ServerResponse *server_res;
        Chat__MessageCommunication *msg;

        server_res = chat__server_response__unpack(NULL, strlen(buff_out), buff_out);

        //Get Response Code
        int code = server_res->code;
        int option = (server_res->option);
        if (code == 200 && option == 1)
        {
            printf("%s\n", server_res->servermessage);   
        }
        else
        {
            //Print Error Message
            printf("%s\n", server_res->servermessage);
            return EXIT_FAILURE;
        }
    }
    else
    {
        return EXIT_FAILURE;
    }

    bzero(buff_out, BUFFER_SZ);



    pthread_t send_msg_thread;
    if (pthread_create(&send_msg_thread, NULL, (void *)client_menu_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    pthread_t recv_msg_thread;
    if (pthread_create(&recv_msg_thread, NULL, (void *)recv_msg_handler, NULL) != 0)
    {
        printf("ERROR: pthread\n");
        return EXIT_FAILURE;
    }

    while (1)
    {
        if (flag)
        {
            printf("\nBye\n");
            break;
        }
    }

    close(sockfd);

    return EXIT_SUCCESS;
}