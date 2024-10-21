#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


using namespace std;

int main(){

    int ServerSocket = socket(AF_INET,SOCK_STREAM,0);
    int opt = 1;

    sockaddr_in ServerAddress;

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(9090);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;

    if (setsockopt(ServerSocket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
       perror("setsockopt");
       exit(EXIT_FAILURE);
   }

    if(bind(ServerSocket,(struct sockaddr*)&ServerAddress,sizeof(ServerAddress))<0){
        std::cout << "Bind Failed" << std::endl;
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    listen(ServerSocket,5);
    
    std::cout << "TCP-Server Side" << std::endl;
    int NewSocket;

    if((NewSocket = accept(ServerSocket,nullptr,nullptr))<0){
            std::cout << "new ServerSocket Failed" << std::endl;
            perror("new ServerSocket Failed");
            exit(EXIT_FAILURE);
    }


    while(true){
        char buffer[1024] = {0};
        memset(buffer,0,sizeof(buffer));

        read(NewSocket,buffer,1024);

        std::cout << "Message from Client: " << buffer << std::endl;

        if(strcmp(buffer,"exit") == 0){
            std::cout  << "Closing!!!!!!!" << std::endl;
            break;
        }

        std:string message;
        std::getline(std::cin,message);
        send(NewSocket,message.c_str(),message.length(),0);

        if(message == "exit"){
            std::cout << "Closing..........." << std::endl;
            break;
        }
    }
    close(NewSocket);
    
    close(ServerSocket);

    return 0;
}
