#include "server.hpp"

Server::Server(int domain,int type,int protocol,int port,u_long interface,int backlog){
    this->ls=new Listen(domain,type,protocol,port,interface,backlog);
}

Listen* Server::get_listen(){
    return this->ls;
}