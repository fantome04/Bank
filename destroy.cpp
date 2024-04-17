#include <iostream>
#include <cstdlib>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>

#include "bank.h"

int main()
{
    const char* sem_name = "/sem_shared_mem";
    const char* shm_name = "/bank_shared_mem";

    if(shm_unlink(shm_name) < 0)
    {
        std::cerr << "shm_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }

    if(sem_unlink(sem_name) < 0) {
        std::cerr << "sem_unlink" << std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}
