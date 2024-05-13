#include "bind.hpp"

class Listen: public Bind{
    private:
        int backlog;
    public:
        Listen(int domain,int type,int protocol,int port,u_long interface,int backlog);
        int listen_to_network(int);
};