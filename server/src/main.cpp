#include "server.h"
#include <iostream>
#include <cstdlib>

int main() {
    const char* port_str = std::getenv("PORT");
    int port = port_str ? std::atoi(port_str) : 8080;
    Server server(port);
    std::cout << "Starting server on port " << port << "..." << std::endl;
    server.start();
    return 0;
}
