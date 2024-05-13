#include "listen.hpp"

Listen::Listen(int domain,int type,int protocol,int port,u_long interface,int backlog):Bind(domain,type,protocol,port,interface){
    std::cout<<"Listening on port "<<port<<std::endl;
    this->backlog=backlog;
    this->listen_to_network(this->get_sockfd());
};

int Listen::listen_to_network(int sockfd) {
    int connection=listen(sockfd,this->backlog);
    if (connection < 0) {
        perror("Error listening");
        exit(1);
    }
    return connection;
}