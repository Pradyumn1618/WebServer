#include "server.hpp"

class Test:public Server{
    private:
        void accept_connection();
        void respond_connection(int);
        int new_sock;
    public:
        Test();
        void start();
        std::string parseRequest(const std::string buffer);
};
