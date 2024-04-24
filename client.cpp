#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <sstream>
#include <semaphore.h>
#include <string>

#include "bank.h"
#include "config.h"

std::string logic(Bank* ptr, std::string input);

int main()
{
    sem_t* sem = sem_open(sem_name,  O_CREAT, 0666, 1);
    if(sem == SEM_FAILED) {
        std::cerr << "open" << std::endl;
        exit(errno);
    }

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open client" <<std::endl;
        exit(errno);
    }

    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    Bank* ptr = (Bank*)mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }
    
    while(true)
    {
        std::string input;
        std::getline(std::cin, input);
        if(input == "exit")
            break;

        if(sem_wait(sem) < 0) {
            std::cerr << "wait" << std::endl;
            exit(errno);
        }

        std::string res = logic(ptr, input);

        if(sem_post(sem) < 0) {
            std::cerr << "post" << std::endl;
            exit(errno);
        }
        std::cout << res << std::endl;
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

std::string logic(Bank* ptr, std::string input)
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
