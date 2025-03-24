#include "server.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>

void handle_client(int client_socket) {
    char buffer[1024] = {0};
    read(client_socket, buffer, 1024);
    std::cout << "Received request:\n" << buffer << std::endl;
    
    // Simple HTTP response for demonstration.
    const char* http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"
                                  "<html><body><h1>Hello from Library Management Server</h1></body></html>";
    write(client_socket, http_response, strlen(http_response));
    close(client_socket);
}

Server::Server(int port) : port(port) {}

Server::~Server() {}

void Server::start() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    // Create socket file descriptor.
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    
    // Set socket options separately for compatibility on macOS.
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEADDR");
        exit(EXIT_FAILURE);
    }
    
    #ifdef SO_REUSEPORT
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) < 0) {
        perror("setsockopt SO_REUSEPORT");
        exit(EXIT_FAILURE);
    }
    #endif

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    if (listen(server_fd, 3) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Server listening on port " << port << std::endl;
    
    while (true) {
        if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        // Handle each client in a separate thread.
        std::thread client_thread(handle_client, client_socket);
        client_thread.detach();
    }
}
