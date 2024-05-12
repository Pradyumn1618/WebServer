#include "socket.hpp"

Socket::Socket(int domain,int type,int protocol,int port,u_long interface) {
    this->port = port;
    this->server_addr.sin_addr.s_addr = htonl(interface);
    this->server_addr.sin_family = domain;
    this->server_addr.sin_port = htons(this->port);
    this->server = gethostbyname("localhost");

    this->sockfd = socket(domain,type,protocol);
    if (this->sockfd < 0) {
        perror("Error opening socket");
        exit(1);
    }
    if (this->server == NULL) {
        fprintf(stderr,"Error, no such host\n");
        exit(1);
    }
    bzero((char *) &this->server_addr,sizeof(this->server_addr));
}

int Socket::get_connection() {
    return this->connection;
}

struct sockaddr_in Socket::get_address() {
    return this->server_addr;
}

int Socket::get_sockfd() {
    return this->sockfd;
}