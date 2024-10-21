// udp_server.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <vector>

#define PORT 9090
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 5

struct ClientInfo {
    struct sockaddr_in addr;
    socklen_t addrlen;
    int client_number;
};

void handle_client(int server_fd, ClientInfo client_info) {
    char buffer[BUFFER_SIZE] = {0};
    std::string filename = "received_file_" + std::to_string(client_info.client_number) + ".txt";

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error creating file for client " << client_info.client_number << std::endl;
        return;
    }

    ssize_t bytes_received;
    while (true) {
        bytes_received = recvfrom(server_fd, buffer, BUFFER_SIZE, 0, (struct sockaddr*)&client_info.addr, &client_info.addrlen);
        if (bytes_received < 0) {
            std::cerr << "Error receiving data from client " << client_info.client_number << std::endl;
            break;
        } else if (bytes_received == 0) {
            break; // Exit loop if no data is received (optional)
        }
        outfile.write(buffer, bytes_received);
    }

    std::cout << "File received successfully as " << filename << "." << std::endl;
    outfile.close();
}

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set socket options
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for connections..." << std::endl;

    std::vector<ClientInfo> clients;

    // Accept a maximum of 5 clients
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        ClientInfo client_info;
        client_info.addrlen = sizeof(client_info.addr);

        // Receive the first message to establish the client
        ssize_t bytes_received = recvfrom(server_fd, nullptr, 0, 0, (struct sockaddr*)&client_info.addr, &client_info.addrlen);
        if (bytes_received < 0) {
            perror("Receive failed");
            continue;
        }

        client_info.client_number = i; // Use 1-based indexing for client numbers
        clients.push_back(client_info);

        // Handle the client
        handle_client(server_fd, client_info);
    }

    close(server_fd);
    std::cout << "All clients have been served. Server shutting down." << std::endl;
    return 0;
}
