// #include <cstring>
// #include <iostream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>

// using namespace std;

// int main(){

//     int ClientSocket = socket(AF_INET,SOCK_STREAM,0);
     
//     sockaddr_in ServerAddress;

//     ServerAddress.sin_family = AF_INET;
//     ServerAddress.sin_port = htons(9090);
//     ServerAddress.sin_addr.s_addr = INADDR_ANY;

//     if(connect(ClientSocket,(struct sockaddr*)&ServerAddress,sizeof(ServerAddress))<0){
//         std::cout << "Connection Failed......." << std::endl;
//         return -1;
//     }
//     std::cout << "TCP-Client Side" << std::endl;

//     while(true){
//         std:string message;
//         std::getline(std::cin,message);

//         send(ClientSocket,message.c_str(),message.length(),0);

//         if(message == "exit"){
//             std::cout << "Closing..........." << std::endl;
//             break;
//         }

//         char buffer[1024] ={0};

//         read(ClientSocket,buffer,1024);

//         std::cout << "Message from Server: " << buffer <<  std::endl;

//         if(strcmp(buffer,"exit") == 0){
//             std::cout  << "Closing!!!!!!!" << std::endl;
//             break;
//         }

//     }

//     close(ClientSocket);

//     return 0;
// }



// client.cpp
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
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        std::cout << "Socket creation error" << std::endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        std::cout << "Invalid address or address not supported" << std::endl;
        return -1;
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cout << "Connection failed" << std::endl;
        return -1;
    }

    std::ifstream infile(file_to_send, std::ios::binary);
    if (!infile) {
        std::cerr << "Error opening file" << std::endl;
        return 1;
    }

    // Send file data
    while (infile.read(buffer, BUFFER_SIZE) || infile.gcount()) {
        send(sock, buffer, infile.gcount(), 0);
    }

    std::cout << "File '" << file_to_send << "' sent successfully." << std::endl;

    // Clean up
    infile.close();
    close(sock);

    return 0;
}
