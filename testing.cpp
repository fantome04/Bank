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
sem_t *sems;
Bank* ptr;
int* id;
int main()
{
    const int n = 10;
    const char* sem_name = "/sem_shared_mem";
    int sem_shm = shm_open(sem_name,  O_RDWR, 0666);
    if(sem_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }

    sems = (sem_t*)mmap(nullptr, n*sizeof(sem_t), PROT_READ | PROT_WRITE, MAP_SHARED, sem_shm, 0);


    const char* client_name = "/client_id_shm";
    int client_shm = shm_open(client_name, O_RDWR, 0666);
    if(client_shm == -1)
    {
        std::cerr <<"sem shm open"<<std::endl;
        exit(errno);
    }
    id = (int*)mmap(nullptr, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, client_shm, 0);
    (*id)++;
    i = *id - 1;
    if(sem_init(&sems[(*id) - 1], 1, 1) == -1)
    {
        std::cerr << "sem_init" <<std::endl;

        exit(errno);
    }


    const char* shm_name = "/bank_shared_mem";

    int shm_fd = shm_open(shm_name, O_RDWR, 0666);
    if(shm_fd == -1)
    {
        std::cerr << "shm_open" <<std::endl;
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

    
    if (sem_destroy(&sems[i]) == -1) {
        std::cerr << "sem_destroy";
        exit(EXIT_FAILURE);
    }
    (*id)--;
    if(munmap(ptr, size) == -1)
    {
        std::cerr << "init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(sems, n * sizeof(sem_t)) == -1)
    {
        std::cerr << "sem init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }

    if(munmap(id, sizeof(int)) == -1)
    {
        std::cerr << "id init munmap" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(shm_fd) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(sem_shm) == -1)
    {
        std::cerr << "init close" <<std::endl;
        exit(EXIT_FAILURE);
    }
    if(close(client_shm) == -1)
    {
        std::cerr << "close" <<std::endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

std::string logic(std::string input)
{
    std::string str;
    std::stringstream strr(input);
    std::string temp;
    std::vector<std::string> in;
    while(strr >> temp) {
        in.push_back(temp);
    }
    
    if(in.size() == 2)
    {
        if(in[0] == "current")
        {
            sem_wait(&sems[i]);
            int res = ptr->get_cell_curr_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Current balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(&sems[i]);
        }
        else if(in[0] == "minimum")
        {
            sem_wait(&sems[i]);
            int res = ptr->get_cell_min_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Minimum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(&sems[i]);
        }
        else if(in[0] == "maximum")
        {
            sem_wait(&sems[i]);
            int res = ptr->get_cell_max_balance(std::stoi(in[1]) - 1);
            if(res == -1)
                str = str + "invalid id";
            else {
                str = str + "Maximum balance for cell number ";
                str = str + in[1];
                str = str + ": ";
                str = str + std::to_string(res);
            }
            sem_post(&sems[i]);
        }
        else if(in[0] == "freeze")
        {
            sem_wait(&sems[i]);
            bool res = ptr->freeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number " + in[1]+" successfully frozen";
            sem_post(&sems[i]);
        }
        else if(in[0] == "unfreeze")
        {
            sem_wait(&sems[i]);
            bool res = ptr->unfreeze_cell(std::stoi(in[1]) - 1);
            if(!res)
                str = str + "invalid id";
            else
                str = str + "Cell number "+in[1]+" successfully unfrozen";
            sem_post(&sems[i]);
        }
        else if(in[0] == "addall")
        {
            sem_wait(&sems[i]);
            bool res = ptr->add_to_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully added " +in[1] +" to all cells";
            sem_post(&sems[i]);
        }
        else if(in[0] == "suball")
        {
            sem_post(&sems[i]);
            bool res = ptr->sub_from_all(std::stoi(in[1]));
            if(!res)
                str = str + "invalid op";
            else
                str = str + "Successfully subbed " + in[1] +" from all cells";
            sem_post(&sems[i]);
        }
        else
        {
            sem_wait(&sems[i]);
            str = str + "invalid input";
            sem_post(&sems[i]);
        }
    }
    else if(in.size() == 3)
    {
        if(in[0] == "setmin")
        {
            sem_wait(&sems[i]);
            bool res = ptr->set_cell_min_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Minimum for cell "+in[1]+" is set to "+in[2];
            sem_post(&sems[i]);
        }
        else if(in[0] == "setmax")
        {
            sem_wait(&sems[i]);
            bool res = ptr->set_cell_max_amount(std::stoi(in[1]) - 1, std::stoi(in[2]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Maximum for cell "+in[1]+" is set to "+in[2];
            sem_post(&sems[i]);
        }
        else
        {
            sem_wait(&sems[i]);
            str = str + "invalid input";
            sem_post(&sems[i]);
        }
    }
    else if(in.size() == 4)
    {
        if(in[0] == "transfer")
        {
            sem_wait(&sems[i]);
            bool res = ptr->transfer(std::stoi(in[1]) - 1, std::stoi(in[2]) - 1, std::stoi(in[3]));
            if(!res)
                str = str + "invalid op";
            else
                str = str +"Successfully transferred "+in[3] +" from " +in[1]+" to "+in[2];
            sem_post(&sems[i]);
        }
        else
        {
            sem_wait(&sems[i]);
            str = str + "invalid input";
            sem_post(&sems[i]);
        }
    }
    else
    {
        sem_wait(&sems[i]);
        str = str + "invalid input";
        sem_post(&sems[i]);
    }

    return str;
}
