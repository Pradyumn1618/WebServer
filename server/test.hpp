#include "server.hpp"

class Test:public Server{
    private:
        char buffer[1024];
        void accept_connection();
        void handle_connection();
        void respond_connection();
        int new_sock;
    public:
        Test();
        void start();
};
