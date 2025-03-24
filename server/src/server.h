#ifndef SERVER_H
#define SERVER_H

class Server {
public:
    Server(int port);
    ~Server();
    void start();
private:
    int port;
    // Additional members (like socket file descriptor) can be added here.
};

#endif // SERVER_H
