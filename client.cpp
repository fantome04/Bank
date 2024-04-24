#include <iostream>
#include <unistd.h>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string>

int main(){
    struct sockaddr_in server_address;

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(errno);
    }

    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9888);

    int connected = connect(client_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    if(connected == -1){
        perror("connect");
        exit(errno);
    }

    std::string mssg;
    char buffer[3001];

    while(true) {
        std::getline(std::cin, mssg);
        if(mssg == "exit") {
            break;
        }

        int sent = send(client_socket, mssg.c_str(), mssg.size(), 0);
        if(sent == -1){
            exit(EXIT_FAILURE);
        }

        int rs = recv(client_socket, buffer, 3000, 0);
        buffer[rs] = '\0';
        if (rs == -1) {
            perror("receive");
            exit(EXIT_FAILURE);
        }

        if(rs > 0) {
            std::cout << buffer << std::endl;
        }
    }

    if(close(client_socket) < 0){
        perror("close");
        exit(errno);
    }
    return 0;
}