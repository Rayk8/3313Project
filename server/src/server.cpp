#include "server.h"
#include "auth.h"
#include <iostream>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <cstring>
#include <cstdlib>
#include <string>

void handleClient(int client_socket, Library* library) {
    char buffer[2048] = {0};
    int bytesRead = read(client_socket, buffer, 2048);
    if (bytesRead <= 0) {
        close(client_socket);
        return;
    }
    std::string request(buffer);
    std::string response;

    if (request.find("GET /catalog") != std::string::npos) {
        response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + library->getCatalog();
    }
    else if (request.find("GET /search") != std::string::npos) {
        size_t pos = request.find("query=");
        if (pos != std::string::npos) {
            size_t end = request.find(" ", pos);
            std::string query = request.substr(pos + 6, end - (pos + 6));
            response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n" + library->searchBooks(query);
        } else {
            response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nMissing query parameter";
        }
    }
    else if (request.find("GET /login") != std::string::npos) {
        size_t posUser = request.find("username=");
        size_t posPass = request.find("password=");
        if (posUser != std::string::npos && posPass != std::string::npos) {
            size_t endUser = request.find("&", posUser);
            std::string username = request.substr(posUser + 9, endUser - (posUser + 9));
            size_t endPass = request.find(" ", posPass);
            std::string password = request.substr(posPass + 9, endPass - (posPass + 9));
            if (loginUser(username, password)) {
                // response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nLogin Successful";
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\nLogin Successful";

            } else {
                response = "HTTP/1.1 401 Unauthorized\r\nContent-Type: text/html\r\n\r\nLogin Failed";
            }
        } else {
            response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nMissing login parameters";
        }
    }
    else if (request.find("GET /register") != std::string::npos) {
        size_t posUser = request.find("username=");
        size_t posPass = request.find("password=");
        if (posUser != std::string::npos && posPass != std::string::npos) {
            size_t endUser = request.find("&", posUser);
            std::string username = request.substr(posUser + 9, endUser - (posUser + 9));
            size_t endPass = request.find(" ", posPass);
            std::string password = request.substr(posPass + 9, endPass - (posPass + 9));
            if (registerUser(username, password)) {
                // response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nRegistration Successful";
                response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nAccess-Control-Allow-Origin: *\r\n\r\nLogin Successful";
            } else {
                response = "HTTP/1.1 409 Conflict\r\nContent-Type: text/html\r\n\r\nUser Already Exists";
            }
        } else {
            response = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/html\r\n\r\nMissing registration parameters";
        }
    }
    else {
        response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\nInvalid Endpoint";
    }

    write(client_socket, response.c_str(), response.length());
    close(client_socket);
}

Server::Server(int port) : port(port) {}

Server::~Server() {}

void Server::start() {
    int server_fd, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt failed");
        exit(EXIT_FAILURE);
    }

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
        client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (client_socket < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }
        std::thread t(handleClient, client_socket, &library);
        t.detach();
    }
}
