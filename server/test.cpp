#include "test.hpp"

Test::Test():Server(AF_INET,SOCK_STREAM,0,8080,INADDR_ANY,5){
    std::cout<<"Server started"<<std::endl;
    start();
}
void Test::accept_connection(){
    struct sockaddr_in client_addr=this->get_listen()->get_address();
    int len=sizeof(client_addr);
    this->new_sock=accept(this->get_listen()->get_sockfd(),(struct sockaddr*)&client_addr,(socklen_t*)&len);
    if (new_sock < 0) {
        perror("Error accepting connection");
        exit(1);
    }
    read(this->new_sock,this->buffer,1024);
}

void Test::handle_connection(){
    std::cout<<"Received: "<<this->buffer<<std::endl;
    
}

void Test :: respond_connection(){
    write(this->new_sock,"Hello",5);
    close(this->new_sock);
}

void Test::start(){
    while(1){
        std::cout<<"Waiting for connection..."<<std::endl;  
        this->accept_connection();
        this->handle_connection();
        this->respond_connection();
        std::cout<<"Connection closed"<<std::endl;
    }
}