#ifndef BANKCELL
#define BANKCELL

#include <semaphore.h>

class BankCell{
public:
    BankCell();
    int get_min_balance() const;
    int get_max_balance() const;
    int get_curr_balance() const;
    void freeze();
    void unfreeze();
    bool is_frozen();
    bool send_amount(int amount);
    bool receive_amount(int amount);
    void set_min_amount(int amount);
    void set_max_amount(int amount);

public:
    int current_balance;
    int min_amount = 0;
    int max_amount = 1000;
    bool frozen;
};

#endif

