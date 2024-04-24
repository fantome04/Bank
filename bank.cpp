#include "bank.h"
#include <iostream>

int Bank::get_cell_curr_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    return cells[num].get_curr_balance();
}

int Bank::get_cell_min_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    return cells[num].get_min_balance();
}

int Bank::get_cell_max_balance(int num) const {
    if(num < 0 || num >= bankSize) {
        return -1;
    }

    return cells[num].get_max_balance();
}

bool Bank::freeze_cell(int num) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    cells[num].freeze();

    return true;
}

bool Bank::unfreeze_cell(int num) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    cells[num].unfreeze();

    return true;
}

bool Bank::transfer(int a, int b, int amount) {
    if(a < 0 || a >= bankSize || b < 0 || b >= bankSize || amount < 0) {
        return false;
    }

    if(cells[a].is_frozen() || cells[b].is_frozen()) {
        return false;
    }

    if(cells[a].send_amount(amount)) {
        if(cells[b].receive_amount(amount)) {
            return true;
        }

        cells[a].receive_amount(amount);
    }

    return false;
}

bool Bank::add_to_all(int amount) {
    if(amount < 0) {
        return false;
    }

    int index = 0;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].receive_amount(amount) ) {
            for(int i = 0; i < index; ++i) {
                cells[i].send_amount(amount);
            }

            return false;
        }
    }

    return true;
}

bool Bank::sub_from_all(int amount) {
    if(amount < 0) {
        return false;
    }

    int index = 0;
    for( ; index < bankSize; ++index) {
        if(cells[index].is_frozen() || !cells[index].send_amount(amount)) {
            for(int i = 0; i < index; ++i) {
                cells[i].receive_amount(amount);
            }

            return false;
        }
    }

    return true;
}

bool Bank::set_cell_min_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    if(amount > cells[num].get_curr_balance() || amount >= cells[num].get_max_balance() || cells[num].is_frozen()) {
        return false;
    }

    cells[num].set_min_amount(amount);

    return true;
}

bool Bank::set_cell_max_amount(int num, int amount) {
    if(num < 0 || num >= bankSize) {
        return false;
    }

    if(amount < cells[num].get_curr_balance() || amount <= cells[num].get_min_balance() || cells[num].is_frozen()) {
        return false;
    }

    cells[num].set_max_amount(amount);

    return true;
}

BankCell& Bank::operator[](unsigned int ind)
{
    return cells[ind];
}
