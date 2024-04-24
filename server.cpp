#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cerrno>
#include <sstream>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <thread>
#include <vector>


#include "bank.h"

sem_t* sem;
Bank* ptr;

std::string logic(std::string input);

void client_handler(int client_socket);

int main()
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(server_socket == -1){
        perror("socket");
        exit(errno);
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(9888);

    if(bind(server_socket, (struct sockaddr*)& server_address, sizeof(server_address)) < 0){
        perror("bind");
        exit(errno);
    }

    if(listen(server_socket, 1024) < 0){
        perror("listen");
        exit(errno);
    }

    std::cout << "waiting for connection\n";

    const char* sem_name = "/sem_shared_mem";
    sem = sem_open(sem_name,  O_CREAT, 0666, 1);
    if(sem == SEM_FAILED) {
        std::cerr << "open" << std::endl;
        exit(errno);
    }

    const int n = 10;

    const char* shm_name = "/bank_shared_mem";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open client" <<std::endl;
        exit(errno);
    }

    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::vector<std::thread> client_threads;

    while(true){
        int client_socket;
        struct sockaddr_in client_address;
        unsigned int client_addr_len = sizeof(client_address);

        if((client_socket = accept(server_socket, (struct sockaddr*) &client_address, &client_addr_len)) < 0){
            perror("accept");
            exit(errno);
        }

        std::cout << "Connected client with address: " << inet_ntoa(client_address.sin_addr) << "\n";

        client_threads.push_back(std::thread(client_handler, client_socket));
    }

    for (auto &thread : client_threads) {
        thread.join();
    }

    if(close(server_socket) < 0){
        perror("close");
        exit(errno);
    }

    if(sem_close(sem) < 0) {
        std::cerr << "close" << std::endl;
        exit(errno);
    }

    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }


    if(close(shm_fd) == -1) {
        std::cerr << "init close" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

void client_handler(int client_socket){
    char buffer[3001];
    int rs;

    while((rs = recv(client_socket, buffer, 3000, 0)) > 0){
        if(sem_wait(sem) < 0) {
            std::cerr << "wait" << std::endl;
            return;
        }

        buffer[rs] = '\0';

        std::string mssg =  logic(buffer);

        int sent = send(client_socket, mssg.c_str(), mssg.size(), 0);
        if(sent == -1){
            return;
        }

        if(sem_post(sem) < 0) {
            std::cerr << "post" << std::endl;
            return;
        }
    }
    if (rs == -1) {
        perror("receive");
        return;
    }

    if(close(client_socket) < 0){
        perror("close");
        return;
    }
}

std::string logic(std::string input)
{
    std::stringstream string(input);
    std::string temp;
    std::vector<std::string> in;
    while(string >> temp) {
        in.push_back(temp);
    }
    std::string str;

    if(in.size() == 2)
    {
        if(in[0] == "current")
        {
            int res = ptr->get_cell_curr_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else
                str = str + "Current balance for cell number " + in[1] + ": " + std::to_string(res);
        }
        else if(in[0] == "minimum")
        {
            int res = ptr->get_cell_min_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else
                str = str + "Minimum balance for cell number " + in[1] + ": " + std::to_string(res);
        }
        else if(in[0] == "maximum")
        {
            int res = ptr->get_cell_max_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else
                str = str + "Maximum balance for cell number " + in[1] + ": " + std::to_string(res);
        }
        else if(in[0] == "freeze")
        {
            bool res = ptr->freeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number " + in[1] + " successfully frozen";
        }
        else if(in[0] == "unfreeze")
        {
            bool res = ptr->unfreeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number " + in[1] + " successfully unfrozen";
        }
        else if(in[0] == "addall")
        {
            bool res = ptr->add_to_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Successfully added " + in[1] + " to all cells";
        }
        else if(in[0] == "suball")
        {
            bool res = ptr->sub_from_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Successfully subbed " + in[1] + " from all cells";
        }
        else if(in[0] == "getinfo") {
            std::string res = ptr->get_info(std::stoi(in[1]) - 1);
            if(res == "")
                str = str + "invalid id";
            else
                str = str + res;
        }
        else
        {
            str = str + "invalid id";
        }
    }
    else if(in.size() == 3)
    {
        if(in[0] == "setmin")
        {
            bool res = ptr->set_cell_min_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid operation";
            else
                str = str + "Minimum for cell " + in[1] + " is set to "+ in[2];
        }
        else if(in[0] == "setmax")
        {
            bool res = ptr->set_cell_max_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid operation";
            else
                str = str + "Maximum for cell " + in[1] + " is set to " + in[2];
        }
        else
        {
            str = str + "invalid input";
        }
    }
    else if(in.size() == 4)
    {
        if(in[0] == "transfer")
        {
            bool res = ptr->transfer(std::stoi(in[1]) - 1, std::stoi(in[2]) - 1, std::stoi(in[3]));
            if(!res)
                str = str + "invalid operation";
            else
                str = str + "Successfully transferred " + in[3] + " from " + in[1] + " to " + in[2];
        }
        else
        {
            str = str + "invalid input";
        }
    }
    else {
        str = str + "invalid input";
    }
    return str;
}
