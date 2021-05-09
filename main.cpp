#include <iostream>
#include "chat.pb.h"
#include <fstream>

using namespace std;


extern "C"{
    #include <stdio.h>
    #include <sys/socket.h>
    #include <netdb.h>
    #include <unistd.h>
    #include <string.h>

    int mainInC(int argc, char *ip, char *portArg, char *message)
    {
        int port;
        int sock = -1;
        struct sockaddr_in address;
        struct hostent * host;
        int len;
        char selectedOption[5];
        /* obtain port number */
        if (sscanf(portArg, "%d", &port) <= 0)
        {
            fprintf(stderr, "%s: error: wrong parameter: port\n", "client");
            return -2;
        }

        /* create socket */
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock <= 0)
        {
            fprintf(stderr, "%s: error: cannot create socket\n", "client");
            return -3;
        }

        /* connect to server */
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        host = gethostbyname(ip);
        if (!host)
        {
            fprintf(stderr, "%s: error: unknown host %s\n", "client", ip);
            return -4;
        }
        memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
        if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
        {
            fprintf(stderr, "%s: error: cannot connect to host %s\n", "client", ip);
            return -5;
        }

        /* send text to server */
        len = strlen(message);
        write(sock, &len, sizeof(int));
        write(sock, message, len);

        /* close socket */
        close(sock);

        return 0;
    }
}

int main() {
    int i;
    std::cout << "Opciones: 1-7" << std::endl;
    //mainInC(4, "3.138.109.53", "5192", "hola");
    chat::ChatProtocol chat;
    chat.set_ip("1");
    chat.set_username("Rodrigo");
    string binary;
    chat.SerializeToString(&binary);
    chat::ChatProtocol chat2;
    std::cout<< binary<< std::endl;
    chat2.ParseFromString(binary);
    std::cout<< "name" << chat2.username() << endl;
    //std::cout<<"yay"<<std::endl;
    //std::cin >> i;
    //switch (i) {
    //    case 1:
    //        std::cout << "Broadcasting" << std::endl;
    //        break;
    //    case 2:
    //        std::cout << "Private Message" << std::endl;
    //        break;
    //    case 3:
    //        std::cout << "Status change" << std::endl;
    //        break;
    //    case 4:
    //        std::cout << "All users" << std::endl;
    //        break;
    //    case 5:
    //        std::cout << "One User" << std::endl;
    //        break;
    //    case 6:
    //        std::cout << "Help" << std::endl;
    //        break;
    //    case 7:
     //       std::cout << "Log out" << std::endl;
      //      return 0;
    //}
    std::cout<<i;
    return 0;
}
