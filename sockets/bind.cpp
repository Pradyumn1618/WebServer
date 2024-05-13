#include "bind.hpp"

Bind::Bind(int domain,int type,int protocol,int port,u_long interface):Socket(domain,type,protocol,port,interface){
    std::cout<<"Binding to port "<<port<<std::endl;
    this->connection=connect_to_network(this->get_sockfd(),this->get_address());
};

int Bind::connect_to_network(int sockfd,struct sockaddr_in server_addr) {
    int connection;
    connection=bind(sockfd,(struct sockaddr *) &server_addr,sizeof(server_addr));
    if (connection < 0) {
        perror("Error connecting");
        exit(1);
    }
    return connection;
}