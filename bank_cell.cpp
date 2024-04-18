#include "bank_cell.h"
#include <iostream>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <string>

BankCell::BankCell(int num)
        :current_balance(0), num(num), frozen(false) {
    std::string name = std::to_string(num);
    cell_sem = sem_open(name.c_str(), O_CREAT | O_EXCL, 1, 1);
    if(cell_sem == SEM_FAILED) {
        std::cout << "UA" << std::endl;
        exit(errno);
    }
}

BankCell::~BankCell() {
    sem_close(cell_sem);
}

int BankCell::get_min_balance() const {
    return min_amount;
}

int BankCell::get_max_balance() const {
    return max_amount;
}

int BankCell::get_curr_balance() const {
    return current_balance;
}

void BankCell::freeze() {
    frozen = true;
}

void BankCell::unfreeze() {
    frozen = false;
}

bool BankCell::is_frozen() {
    return frozen;
}

bool BankCell::receive_amount(int amount)  {
    if(current_balance + amount <= max_amount) {
        current_balance += amount;
        return true;
    }

    return false;
}

bool BankCell::send_amount(int amount) {
    if(current_balance - amount >= min_amount) {
        current_balance -= amount;
        return true;
    }

    return false;
}

void BankCell::set_min_amount(int amount) {
    min_amount = amount;
}

void BankCell::set_max_amount(int amount) {
    max_amount = amount;
}


