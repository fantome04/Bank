#include "bank.h"
#include <iostream>

int Bank::get_cell_curr_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    std::cout << "vv" << std::endl;


    if(sem_wait(cells[num].cell_sem) < 0){
        std::cout << "wait" <<std::endl;
        exit(errno);
    }

    std::cout << "vv" << std::endl;

    int ans = cells[num].get_curr_balance();

    sem_post(cells[num].cell_sem);

    return ans;
}

int Bank::get_cell_min_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    sem_wait(cells[num].cell_sem);

    int ans = cells[num].get_min_balance();

    sem_post(cells[num].cell_sem);

    return ans;
}

int Bank::get_cell_max_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    sem_wait(cells[num].cell_sem) < 0;

    int ans = cells[num].get_max_balance();

    sem_post(cells[num].cell_sem);

    return ans;
}

bool Bank::freeze_cell(int num) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    sem_wait(cells[num].cell_sem);

    cells[num].freeze();

    sem_post(cells[num].cell_sem);

    return true;
}

bool Bank::unfreeze_cell(int num) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    sem_wait(cells[num].cell_sem);

    cells[num].unfreeze();

    sem_post(cells[num].cell_sem);

    return true;
}

bool Bank::transfer(int a, int b, int amount) {
    if(a < 0 || a >= bankSize || b < 0 || b >= bankSize || amount < 0) {
        return false;
    }

    sem_wait(cells[a].cell_sem);
    sem_wait(cells[b].cell_sem);

    if(cells[a].is_frozen() || cells[b].is_frozen()) {
        sem_post(cells[a].cell_sem);
        sem_post(cells[b].cell_sem);
        return false;
    }

    if(cells[a].send_amount(amount)) {
        if(cells[b].receive_amount(amount)) {
            sem_post(cells[a].cell_sem);
            sem_post(cells[b].cell_sem);
            return true;
        }

        cells[a].receive_amount(amount);
    }

    sem_post(cells[a].cell_sem);
    sem_post(cells[b].cell_sem);

    return false;
}

bool Bank::add_to_all(int amount) {
    for(int i = 0; i < bankSize; ++i) {
        sem_wait(cells[i].cell_sem);
    }

    if(amount < 0) {
        return false;
    }

    int index = 0;
    bool ans = true;
    for( ; index < bankSize; ++index) {
        if(!cells[index].receive_amount(amount) || cells[index].is_frozen()) {
            for(int i = 0; i < index; ++i) {
                cells[i].send_amount(amount);
            }

            ans = false;
            break;
        }
    }

    for(int i = 0; i < bankSize; ++i) {
        sem_post(cells[i].cell_sem);
    }

    return ans;
}

bool Bank::sub_from_all(int amount) {
    for(int i = 0; i < bankSize; ++i) {
        sem_wait(cells[i].cell_sem);
    }

    if(amount < 0) {
        return false;
    }

    int index = 0;
    bool ans = true;
    for( ; index < bankSize; ++index) {
        if(!cells[index].send_amount(amount) || cells[index].is_frozen()) {
            for(int i = 0; i < index; ++i) {
                cells[i].receive_amount(amount);
            }

            ans = false;
            break;
        }
    }

    for(int i = 0; i < bankSize; ++i) {
        sem_post(cells[i].cell_sem);
    }

    return ans;
}

bool Bank::set_cell_min_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    sem_wait(cells[num].cell_sem);

    if(amount > cells[num].get_curr_balance() || amount >= cells[num].get_max_balance() || cells[num].is_frozen()) {
        sem_post(cells[num].cell_sem);
        return false;
    }

    cells[num].set_min_amount(amount);

    sem_post(cells[num].cell_sem);

    return true;
}

bool Bank::set_cell_max_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    sem_wait(cells[num].cell_sem);

    if(amount < cells[num].get_curr_balance() || amount <= cells[num].get_min_balance() || cells[num].is_frozen()) {
        sem_post(cells[num].cell_sem);
        return false;
    }

    cells[num].set_max_amount(amount);

    sem_post(cells[num].cell_sem);

    return true;
}

BankCell& Bank::operator[](unsigned int ind)
{
    return cells[ind];
}
