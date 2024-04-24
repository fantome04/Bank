
#ifndef BANK
#define BANK

#include "bank_cell.h"
#include <vector>
#include <string>
class BankCell;

class Bank{
public:
    int get_cell_curr_balance(int num) const;
    int get_cell_min_balance(int num) const;
    int get_cell_max_balance(int num) const;
    bool freeze_cell(int num);
    bool unfreeze_cell(int num);
    bool transfer(int a, int b, int amount);
    bool add_to_all(int amount);
    bool sub_from_all(int amount);
    bool set_cell_min_amount(int num, int amount);
    bool set_cell_max_amount(int num, int amount);
    std::string get_info(int num);

    unsigned int bankSize;
    BankCell cells[];

    BankCell& operator[](unsigned int ind);
};


#endif
