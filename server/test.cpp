#include "test.hpp"
#include <thread>
#include <vector>
#include <fstream>
#include <iterator>
#include <fcgiapp.h>
#include <fcgi_stdio.h>
#include <sys/wait.h>

#define PORT 8000

std::string get_content_type(const std::string &filePath)
{
    if (filePath.substr(filePath.length() - 5).compare(".html")==0)
    {
        return "text/html";
    }
    else if (filePath.substr(filePath.length() - 4) == ".css")
    {
        return "text/css";
    }
    else if (filePath.substr(filePath.length() - 3) == ".js")
    {
        return "application/javascript";
    }
    else if (filePath.substr(filePath.length() - 4) == ".jpg")
    {
        return "image/jpeg";
    }
    else if (filePath.substr(filePath.length() - 4) == ".png")
    {
        return "image/png";
    }
    else
    {
        // Default to application/octet-stream for unknown file types
        return "application/octet-stream";
    }
}

void handle_php(int new_sock, std::string filePath)
{
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    int child_pid = fork();
    if (child_pid == 0)
    {
        close(pipefd[0]);               // Close read end in the child
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the pipe
        close(pipefd[1]);               // This is no longer needed

        std::cout << "PHP file requested" << std::endl;
        if (FCGX_Init() != 0)
        {
            perror("Error initializing FastCGI");
            exit(EXIT_FAILURE);
        }
        execl("/opt/lampp/bin/php-cgi", "php-cgi", filePath.c_str(), NULL);
        perror("Error executing php-cgi");
    }
    else if (child_pid > 0)
    {
        close(pipefd[1]); // Close write end in the parent

        int status;
        waitpid(child_pid, &status, 0);

        // Read the output of php-cgi from the pipe
        char buffer[100000];
        int bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1);
        if (bytesRead >= 0)
        {
            buffer[bytesRead] = 0; // Null-terminate the string
        }
        else
        {
            // Handle the error
            perror("read");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]); // Close the pipe
        // Prepare the HTTP response
        std::string output(buffer);
        output = output.substr(output.find("\r\n\r\n") + 4); // Remove the HTTP headers
        std::string response = "HTTP/1.1 200 OK\r\n"
                               "Content-Type: text/html\r\n"
                               "Content-Length: " +
                               std::to_string(output.length()) + "\r\n"
                                                                 "Connection: close\r\n"
                                                                 "\r\n" +
                               output;

        // Send the response back to the client
        // std::cout << output << std::endl;
        send(new_sock, response.c_str(), response.length(), 0);
        write(new_sock, buffer, bytesRead); 
        // close(new_sock);
        return;
    }
    else
    {
        perror("Error forking process");
        exit(EXIT_FAILURE);
    }
}

void handle_post(int new_sock, char buffer[], std::string filePath){
   
        // Find the Content-Length header
        
        
        std::string contentLengthHeader = "Content-Length: ";
        size_t contentLengthPos = std::string(buffer).find(contentLengthHeader);
        if (contentLengthPos != std::string::npos)
        {
        //     size_t contentLengthStart = contentLengthPos + contentLengthHeader.length();
        //     size_t contentLengthEnd = std::string(buffer).find("\r\n", contentLengthStart);
        //     std::string contentLengthStr = std::string(buffer).substr(contentLengthStart, contentLengthEnd - contentLengthStart);
        //     int contentLength = std::stoi(contentLengthStr);

        //     // Read the body of the POST request
        //     char* postBody = new char[contentLength + 1];
        //     read(new_sock, postBody, contentLength);
        //     postBody[contentLength] = '\0'; // Null-terminate the string

        //     std::cout << "POST body: " << postBody << std::endl;

        //     // TODO: Add your login logic here
        //     // For example, you might parse the email and password from the POST body,
        //     // check if they match a record in your database, and send a response back to the client

        //     delete[] postBody;
        int pos=std::string(buffer).find("\r\n\r\n");
        std::string postBody = std::string(buffer).substr(pos+4);
        handle_php(new_sock, filePath);
        std::cout << "POST body: " << postBody << std::endl;
        }
        else
        {
            // Invalid request format, return a 400 response
            std::string response = "HTTP/1.1 400 Bad Request\r\n"
                                   "Content-Type: text/plain\r\n"
                                   "Content-Length: 11\r\n"
                                   "Connection: close\r\n"
                                   "\r\n"
                                   "Bad Request";
            send(new_sock, response.c_str(), response.length(), 0);
        }
}



Test::Test() : Server(AF_INET, SOCK_STREAM, 0,PORT, INADDR_ANY, 5)
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
    std::thread(&Test::respond_connection, this, this->new_sock).detach();
}

void Test::respond_connection(int new_sock)
{
    char buffer[1024];
    read(new_sock, buffer, 1024);
    std::cout << "Received: " << buffer << std::endl;
    std::string requestedFile = parseRequest(buffer); // You need to implement this function
    std::string filePath = "./Web_project/" + requestedFile;
    std::cout << "Requested file: " << filePath << std::endl;

    if(std::string(buffer).find("POST") != std::string::npos){
        handle_post(new_sock, buffer, filePath);
        return;
    }

    if (requestedFile.substr(requestedFile.length() - 4).compare(".php") == 0)
    {
        handle_php(new_sock, filePath);
        return;
    }
    else
    {
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
            std::string content_type = get_content_type(filePath);
            std::cout << "Content-Type: " << content_type << std::endl;
            // std::cout << "Content: " << body<< std::endl;

            std::string response = "HTTP/1.1 200 OK\r\n"
                                   "Content-Type: " +
                                   content_type + "\r\n" // You might want to set the correct MIME type based on the requested file
                                                  "Content-Length: " +
                                   std::to_string(body.length()) + "\r\n"
                                                                   "Connection: close\r\n"
                                                                   "\r\n" +
                                   body;
            send(new_sock, response.c_str(), response.length(), 0);
        }
        close(new_sock);
        return;
    }
}

void Test::start()
{
    while (1)
    {
        std::cout << "Waiting for connection..." << std::endl;
        this->accept_connection();
    }
}

std::string Test::parseRequest(const std::string request)
{
    // Find the position of the first space character after the "GET" keyword
    size_t getPos = request.find("GET");
    if (getPos == std::string::npos)
    {
        getPos = request.find("POST");
        if (getPos == std::string::npos)
            return "";
        size_t spacePos = request.find(' ', getPos + 5);
        if (spacePos == std::string::npos)
            return ""; // Invalid request format, return empty string

        // Extract the requested file path
        std::string filePath = request.substr(getPos + 5, spacePos - getPos - 5); // Skip "GET "

        // Remove leading slashes
        while (!filePath.empty() && (filePath[0] == '/' || filePath[0] == '\\'))
            filePath.erase(0, 1);

        return filePath;
    }
    else
    {
        if (getPos == std::string::npos)
            return ""; // Invalid request format, return empty string

        // Find the position of the first space character after the "GET" keyword
        size_t spacePos = request.find(' ', getPos + 4);
        if (spacePos == std::string::npos)
            return ""; // Invalid request format, return empty string

        // Extract the requested file path
        std::string filePath = request.substr(getPos + 4, spacePos - getPos - 4); // Skip "GET "

        // Remove leading slashes
        while (!filePath.empty() && (filePath[0] == '/' || filePath[0] == '\\'))
            filePath.erase(0, 1);
        return filePath;
    }
}
