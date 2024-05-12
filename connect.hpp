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
#include "socket.hpp"

class Connect : public Socket{
    public:
        Connect(int domain,int type,int protocol,int port,u_long interface):Socket(domain,type,protocol,port,interface){};
        int connect_to_network(int,struct sockaddr_in);
        
};