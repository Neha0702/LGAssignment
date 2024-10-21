// #include <cstring>
// #include <iostream>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <vector>

// #define MAX_CLIENT 5

// using namespace std;

// std::vector<int> clients;  // To store client socket descriptors
// pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;



// void * Handle_Clients(void * Client_Socket){

//     int sockClient = *(int*)Client_Socket;

//     while(true){
//         char buffer[1024] = {0};
//         memset(buffer,0,sizeof(buffer));

//         read(sockClient,buffer,1024);

//         std::cout << "Message from Client: " << buffer << std::endl;

//         if(strcmp(buffer,"exit") == 0){
//             std::cout  << "Closing!!!!!!!" << std::endl;
//             break;
//         }

//         std:string message;
//         std::getline(std::cin,message);
//         if(clients.size() == 0){
//             if(message == "exit"){
//                     std::cout << "Closing..........." << std::endl;
//                     break;
//             }
//         }

//         pthread_mutex_lock(&clients_mutex);
//         for (int client : clients) {
            
//                 send(client, message.c_str(), (message.length()), 0);

//                 if(message == "exit"){
//                     std::cout << "Closing..........." << std::endl;
//                     break;
//                 }
            
//         }
//         pthread_mutex_unlock(&clients_mutex); 

//     }

//     close(sockClient);
//     pthread_mutex_lock(&clients_mutex);
//     for (auto it = clients.begin(); it != clients.end(); it++) {
//         it = clients.erase(it); // Erase returns the next iterator
//     }
//     pthread_mutex_unlock(&clients_mutex);

//     free(Client_Socket);
//     pthread_exit(NULL);

// }

// int main(){

//     int ServerSocket = socket(AF_INET,SOCK_STREAM,0);
//     int opt = 1;

//     sockaddr_in ServerAddress;
//     pthread_t Threadid[MAX_CLIENT];
//     int client = 0;

//     ServerAddress.sin_family = AF_INET;
//     ServerAddress.sin_port = htons(9090);
//     ServerAddress.sin_addr.s_addr = INADDR_ANY;

//     if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
//        perror("setsockopt");
//        exit(EXIT_FAILURE);
//    }

//     if(bind(ServerSocket,(struct sockaddr*)&ServerAddress,sizeof(ServerAddress))<0){
//         std::cout << "Bind Failed" << std::endl;
//         perror("bind failed");
//         exit(EXIT_FAILURE);
//     }

//     listen(ServerSocket,5);
    
//     std::cout << "TCP-Server Side" << std::endl;
//     int NewSocket;

//     while(client <= MAX_CLIENT){
//         if((NewSocket = accept(ServerSocket,nullptr,nullptr))<0){
//             std::cout << "new ServerSocket Failed" << std::endl;
//             perror("new ServerSocket Failed");
//             exit(EXIT_FAILURE);
//         }

//         std::cout << "New Client Connnection " << std::endl;

//         pthread_mutex_lock(&clients_mutex);
//         clients.push_back(NewSocket);
//         pthread_mutex_unlock(&clients_mutex);

//         int *sockClient = (int *)malloc(sizeof(int));
//         *sockClient = NewSocket;

//         if(pthread_create(&Threadid[client],NULL,Handle_Clients,(void*)sockClient) != 0){
//             perror("Failed to create thread");
//             exit(EXIT_FAILURE);
//         }
//         client++;
//     }

//     for(int i=0;i<client;i++){
//         pthread_join(Threadid[i],NULL);
//     }
    
//     close(ServerSocket);

//     return 0;
// }






#include <iostream>
#include <fstream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <vector>

#define PORT 9090
#define BUFFER_SIZE 4096
#define MAX_CLIENTS 5

struct ClientInfo {
    int socket;
    int client_number;
};

void* handle_client(void* client_info_ptr) {
    ClientInfo* client_info = (ClientInfo*)client_info_ptr;
    int client_socket = client_info->socket;
    int client_number = client_info->client_number;
    delete client_info; // Clean up dynamically allocated memory

    char buffer[BUFFER_SIZE] = {0};
    std::string filename = "received_file_" + std::to_string(client_number) + ".txt";

    std::ofstream outfile(filename, std::ios::binary);
    if (!outfile) {
        std::cerr << "Error creating file for client " << client_number << std::endl;
        close(client_socket);
        return nullptr;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        outfile.write(buffer, bytes_received);
    }

    if (bytes_received < 0) {
        std::cerr << "Error receiving data from client " << client_number << std::endl;
    } else {
        std::cout << "File received successfully as " << filename << "." << std::endl;
    }

    outfile.close();
    close(client_socket);
    return nullptr;
}

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    std::vector<pthread_t> threads;

    // Create socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    // Attach socket to the port
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Bind the socket to the address
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, MAX_CLIENTS) < 0) {
        perror("Listen");
        exit(EXIT_FAILURE);
    }

    std::cout << "Waiting for connections..." << std::endl;

    // Accept a maximum of 5 clients
    for (int i = 1; i <= MAX_CLIENTS; ++i) {
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket < 0) {
            perror("Accept");
            continue;
        }

        // Create and populate the ClientInfo struct
        ClientInfo* client_info = new ClientInfo;
        client_info->socket = new_socket;
        client_info->client_number = i; // Use 1-based indexing for client numbers

        pthread_t thread;
        if (pthread_create(&thread, nullptr, handle_client, client_info) != 0) {
            std::cerr << "Error creating thread for client " << i << std::endl;
            close(new_socket);
            delete client_info; // Clean up if thread creation fails
        } else {
            threads.push_back(thread);
        }
    }

    // Clean up threads
    for (pthread_t thread : threads) {
        pthread_join(thread, nullptr);
    }

    close(server_fd);
    std::cout << "All clients have been served. Server shutting down." << std::endl;
    return 0;
}
