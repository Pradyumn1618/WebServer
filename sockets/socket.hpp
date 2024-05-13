#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <iostream>
#include <atomic>

class Socket {
    private:
        int sockfd;
        struct sockaddr_in server_addr;
        struct hostent *server;
        int port;
        char *host;
        int timeout;
    public:
        int connection;
        Socket(int domain,int type,int protocol,int port,u_long interface);
        virtual int connect_to_network(int,struct sockaddr_in)=0;
        int get_connection();
        struct sockaddr_in get_address();
        int get_sockfd();
        static void signal_handler(int signum);

};