#include "../sockets/listen.hpp"

class Server{
    private:
        Listen* ls;
        virtual void accept_connection()=0;
        virtual void respond_connection(int)=0;
    public:
        Server(int domain,int type,int protocol,int port,u_long interface,int backlog);
        virtual void start()=0;
        Listen* get_listen();
};