#include "server.h"
#include <iostream>

int main() {
    int port = 8080;
    Server server(port);
    std::cout << "Starting server on port " << port << "..." << std::endl;
    server.start();
    return 0;
}
