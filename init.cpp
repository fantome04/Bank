#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>
#include <semaphore.h>

#include "bank.h"

int main()
{
    const int n = 10;

    const char* shm_name = "/bank_shared_mem";
    int shm_fd = shm_open(shm_name, O_CREAT | O_RDWR , 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open init" <<std::endl;
        exit(errno);
    }
    
    std::size_t size = sizeof(Bank) + n * sizeof(BankCell);

    if(ftruncate(shm_fd, size) == -1)
    {
        std::cerr << "init ftruncate"<<std::endl;
        exit(errno);
    }

    void* ptr = mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if(ptr == MAP_FAILED)
    {
        std::cerr << "init mmap" << std::endl;
        exit(EXIT_FAILURE);
    }

    Bank* bank_ptr = (Bank*)ptr;

    bank_ptr->bankSize = n;

    for (int i = 0 ; i < n; ++i) {
        bank_ptr->cells[i] = BankCell();
    }

    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

