#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

using namespace std;

int main(){

    int ClientSocket = socket(AF_INET,SOCK_STREAM,0);
     
    sockaddr_in ServerAddress;

    ServerAddress.sin_family = AF_INET;
    ServerAddress.sin_port = htons(9090);
    ServerAddress.sin_addr.s_addr = INADDR_ANY;

    if(connect(ClientSocket,(struct sockaddr*)&ServerAddress,sizeof(ServerAddress))<0){
        std::cout << "Connection Failed......." << std::endl;
        return -1;
    }
    std::cout << "TCP-Client Side" << std::endl;

    while(true){
        std:string message;
        std::getline(std::cin,message);

        send(ClientSocket,message.c_str(),message.length(),0);

        if(message == "exit"){
            std::cout << "Closing..........." << std::endl;
            break;
        }

        char buffer[1024] ={0};

        read(ClientSocket,buffer,1024);

        std::cout << "Message from Server: " << buffer <<  std::endl;

        if(strcmp(buffer,"exit") == 0){
            std::cout  << "Closing!!!!!!!" << std::endl;
            break;
        }

    }

    

    close(ClientSocket);

    
    return 0;
}
