#include "socket.hpp"
#include <signal.h>

static Socket *instance = NULL;

Socket::Socket(int domain, int type, int protocol, int port, u_long interface)
{
    this->port = port;
    bzero((char *)&this->server_addr, sizeof(this->server_addr));
    this->server_addr.sin_addr.s_addr = htonl(interface);
    this->server_addr.sin_family = domain;
    this->server_addr.sin_port = htons(this->port);
    std::cout << "Port: " << this->port << " " << this->server_addr.sin_port << std::endl;
    this->server = gethostbyname("0.0.0.0");

    this->sockfd = socket(domain, type, protocol);

    if (this->sockfd < 0)
    {
        perror("Error opening socket");
        exit(1);
    }
    if (this->server == NULL)
    {
        fprintf(stderr, "Error, no such host\n");
        exit(1);
    }
    instance = this;
    int signals[] = {SIGINT, SIGTSTP, SIGTERM, SIGKILL, SIGTRAP, SIGABRT, SIGSEGV, SIGILL};

    // Register the signal handler for each signal
    for (int i = 0; i < 8; i++)
    {
        signal(signals[i], (void (*)(int)) & Socket::signal_handler);
    }
}

int Socket::get_connection()
{
    return this->connection;
}

struct sockaddr_in Socket::get_address()
{
    return this->server_addr;
}

int Socket::get_sockfd()
{
    return this->sockfd;
}

void Socket::signal_handler(int signum)
{
    std::cout << "Interrupt signal (" << signum << ") received.\n";
    close(instance->sockfd);
    exit(signum);
}