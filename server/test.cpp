#include "test.hpp"
#include<thread>
#include<vector>
#include<fstream>
#include<iterator>

Test::Test() : Server(AF_INET, SOCK_STREAM, 0, 8000, INADDR_ANY, 5)
{
    std::cout << "Server started" << std::endl;
    start();
}
void Test::accept_connection()
{
    struct sockaddr_in client_addr = this->get_listen()->get_address();
    int len = sizeof(client_addr);
    this->new_sock = accept(this->get_listen()->get_sockfd(), (struct sockaddr *)&client_addr, (socklen_t *)&len);
    if (new_sock < 0)
    {
        perror("Error accepting connection");
        exit(1);
    }
    std::thread(&Test::respond_connection, this,this->new_sock).detach();
}

void Test::respond_connection(int new_sock)
{
    char buffer[1024];
    read(new_sock, buffer, 1024);
    std::cout << "Received: " << buffer << std::endl;
    std::string requestedFile = parseRequest(buffer); // You need to implement this function
    std::string filePath = requestedFile;
    std::cout<<"Requested file: "<<filePath<<std::endl;

    std::ifstream file;
    file.open(filePath);
    if (!file.is_open())
    {
        perror("Error opening file");

        // If the file doesn't exist, send a 404 response
        std::string response = "HTTP/1.1 404 Not Found\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 9\r\n"
                               "Connection: close\r\n"
                               "\r\n"
                               "Not Found";
        send(new_sock, response.c_str(), response.length(), 0);
    }
    else
    {
        // If the file exists, read it into a string
        std::string body((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        // Send the file as the response
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n" // You might want to set the correct MIME type based on the requested file
                               "Content-Length: " + std::to_string(body.length()) + "\r\n"
                               "Connection: close\r\n"
                               "\r\n" +
                               body;
        send(new_sock, response.c_str(), response.length(), 0);
    }
    close(new_sock);
    return;
    // std::cout << "Connection closed" << std::endl;
}

void Test::start()
{
    while (1)
    {
        std::cout << "Waiting for connection..." << std::endl;
        this->accept_connection();
    }
}


std::string Test:: parseRequest(const std::string request) {
    // Find the position of the first space character after the "GET" keyword
    size_t getPos = request.find("GET");
    if (getPos == std::string::npos)
        return ""; // Invalid request format, return empty string
    
    // Find the position of the first space character after the "GET" keyword
    size_t spacePos = request.find(' ', getPos + 4); // Skip "GET " (4 characters)
    if (spacePos == std::string::npos)
        return ""; // Invalid request format, return empty string
    
    // Extract the requested file path
    std::string filePath = request.substr(getPos + 4, spacePos - getPos - 4); // Skip "GET "
    
    // Remove leading slashes
    while (!filePath.empty() && (filePath[0] == '/' || filePath[0] == '\\'))
        filePath.erase(0, 1);

    return filePath;
}
