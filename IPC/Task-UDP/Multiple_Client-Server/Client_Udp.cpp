// udp_client.cpp
#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 9090
#define BUFFER_SIZE 4096

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file_to_send.txt>" << std::endl;
        return 1;
    }

    const char* file_to_send = argv[1];
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = {0};

    // Create socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address or address not supported" << std::endl;
        return -1;
    }

    // Send a dummy packet to establish the client
    sendto(sock, nullptr, 0, 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    std::ifstream infile(file_to_send, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    // Send file data
    while (infile.read(buffer, BUFFER_SIZE) || infile.gcount()) {
        sendto(sock, buffer, infile.gcount(), 0, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    }

    std::cout << "File '" << file_to_send << "' sent successfully." << std::endl;

    // Clean up
    infile.close();
    close(sock);

    return 0;
}
