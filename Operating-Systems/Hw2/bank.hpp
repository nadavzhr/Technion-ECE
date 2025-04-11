#ifndef _BANK_H
#define _BANK_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <ctime> 
#include <random>
#include "account.hpp"
#include "atm.hpp"

#define TAX_SLEEP_TIME 3 //3sec between tax collection
#define SNAPSHOT_SLEEP_TIME 500000 // 0.5sec between snapshots

using namespace std;
class bank{
    public:
        int bank_balance;
        int num_readers;
        int waiting_writers;
        bool curr_writing;

        vector<account>* all_accounts;
        pthread_mutex_t bank_lock;
        pthread_cond_t read_condition;
        pthread_cond_t write_condition;
        


        void tax_collector();
        void snapshot();

        void bank_r_start();
        void bank_r_end();
        void bank_w_start();
        void bank_w_end();

        bank(vector<account>* all_accounts){
            bank_balance = 0 ;
            num_readers = 0;
            waiting_writers = 0;
            curr_writing = false;
            this->all_accounts = all_accounts;
            pthread_mutex_init(&bank_lock, NULL);
            pthread_cond_init(&read_condition, NULL);
            pthread_cond_init(&write_condition, NULL);


        }



};
// This function will be used as the routine function for the threads.
void* tax_thread_runner(void* bank);
void* snapshot_thread_runner(void* bank);

#endif
