#include "socket.hpp"

class Connect : public Socket{
    public:
        Connect(int domain,int type,int protocol,int port,u_long interface);
        int connect_to_network(int,struct sockaddr_in);
        
};