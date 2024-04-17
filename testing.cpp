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

std::string logic(std::string input);

void testCurrentBalanceValidInput(int num);
void testMinimumBalanceValidInput(int num);
void testMaximumBalanceValidInput(int num);
void testFreezeCellValidInput();
void testUnfreezeCellValidInput();
void testAddToAllValidInput(int num);
void testSubFromAllValidInput(int num);
void testSetMinValidInput();
void testSetMaxValidInput();
void testTransferValidInput();


void testCurrentBalanceValidInput(int num = 0)
{
    std::string input = "current 1";
    std::string expectedOutput = "Current balance for cell number 1: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Current Balance Test Passed" << std::endl;
    } else {
        std::cout << "Current Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Minimum Balance with valid input
void testMinimumBalanceValidInput(int num = 0) {
    std::string input = "minimum 2";
    std::string expectedOutput = "Minimum balance for cell number 2: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Minimum Balance Test Passed" << std::endl;
    } else {
        std::cout << "Minimum Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Maximum Balance with valid input
void testMaximumBalanceValidInput(int num = 1000) {
    std::string input = "maximum 3";
    std::string expectedOutput = "Maximum balance for cell number 3: " + std::to_string(num);
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Maximum Balance Test Passed" << std::endl;
    } else {
        std::cout << "Maximum Balance Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Freeze Cell with valid input
void testFreezeCellValidInput() {
    std::string input = "freeze 4";
    std::string expectedOutput = "Cell number 4 successfully frozen";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Freeze Test Passed" << std::endl;
    } else {
        std::cout << "Freeze Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Unfreeze Cell with valid input
void testUnfreezeCellValidInput() {
    std::string input = "unfreeze 4";
    std::string expectedOutput = "Cell number 4 successfully unfrozen";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Unfreeze Test Passed" << std::endl;
    } else {
        std::cout << "Unfreeze Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Add to All with valid input
void testAddToAllValidInput(int num = 50) {
    std::string input = "addall " + std::to_string(num);
    std::string expectedOutput = "Successfully added " + std::to_string(num) + " to all cells";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Add All Test Passed" << std::endl;
    } else {
        std::cout << "Add All Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}


// Function to test Subtract from All with valid input
void testSubFromAllValidInput(int num = 50) {
    std::string input = "suball " + std::to_string(num);
    std::string expectedOutput = "Successfully subbed " + std::to_string(num) + " from all cells";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput ) {
        std::cout << "Sub All Test Passed" << std::endl;
    } else {
        std::cout << "Sub All Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Set Minimum Amount with valid input
void testSetMinValidInput() {
    std::string input = "setmin 1 50";
    std::string expectedOutput = "Minimum for cell 1 is set to 50";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Set Min Test Passed" << std::endl;
    } else {
        std::cout << "Set Min Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Set Maximum Amount with valid input
void testSetMaxValidInput() {
    std::string input = "setmax 2 100";
    std::string expectedOutput = "Maximum for cell 2 is set to 100";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Set Max Test Passed" << std::endl;
    } else {
        std::cout << "Set Max Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}

// Function to test Transfer with valid input
void testTransferValidInput() {
    std::string input = "transfer 3 4 30";
    std::string expectedOutput = "Successfully transferred 30 from 3 to 4";
    std::cout << "Test Case: " << input << std::endl;
    std::string result = logic(input);
    if (result == expectedOutput) {
        std::cout << "Transfer Test Passed" << std::endl;
    } else {
        std::cout << "Transfer Test Failed. Expected: " << expectedOutput << " Actual: " << result << std::endl;
    }
}





int i;
Bank* ptr;
int* id;

int main()
{
    const char* sem_name = "/sem_shared_mem";
    sem_t* sem = sem_open(sem_name,  O_CREAT, 0666, 1);
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

    testCurrentBalanceValidInput();
    testMinimumBalanceValidInput();
    testMaximumBalanceValidInput();
    testFreezeCellValidInput();
    testUnfreezeCellValidInput();
    testAddToAllValidInput();
    testSubFromAllValidInput();
    testSetMinValidInput();
    testSetMaxValidInput();
    testTransferValidInput();

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


