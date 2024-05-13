
#include "socket.hpp"

class Bind : public Socket{
    public:
        Bind(int domain,int type,int protocol,int port,u_long interface);
        int connect_to_network(int,struct sockaddr_in);
        
};