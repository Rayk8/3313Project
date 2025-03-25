#ifndef SERVER_H
#define SERVER_H

#include "library.h"

class Server {
public:
    Server(int port);
    ~Server();
    void start();
private:
    int port;
    Library library;
};

#endif // SERVER_H
