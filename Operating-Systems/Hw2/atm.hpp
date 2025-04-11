#ifndef _ATM_H
#define _ATM_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <ctime>
#include <unistd.h>
#include <cstdlib>
#include "account.hpp"
#include "bank.hpp"

#define ATM_ACT_TIME 1
#define ATM_SLEEP_TIME 100000 // passed into usleep as microseconds = 0.1 sec

using namespace std;

class atm{
    public:    
        int id;
        string atm_actions_file;
        bank* my_bank;

        void open_func(int account_id, string pass, int initial_amount);
        void deposit_func(int account_id, string pass, int amount);
        void withdraw_func(int account_id, string pass, int amount);
        void balance_func(int account_id, string pass);
        void close_func(int account_id ,string pass);
        void transfer_func(int src_account, string pass, int target_account, int amount);

        void action_handler();

        atm(int id, string atm_actions_file,bank* atm_bank){
            this->id = id;
            this->atm_actions_file = atm_actions_file;
            this->my_bank = atm_bank;
        }
};
#endif
