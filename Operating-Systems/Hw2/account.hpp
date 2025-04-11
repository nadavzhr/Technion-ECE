#ifndef _ACCOUNT_H
#define _ACCOUNT_H
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <cstring>


using namespace std;
class account{
    public:
        int id;
        string pass;
        int balance;
        int num_readers;
        pthread_mutex_t r_lock;
        pthread_mutex_t w_lock;

        void read_lock();
        void read_unlock();
        void write_lock();
        void write_unlock();

        //Account constructor.
        account(int id, string pass, int balance){
            this->id = id;
            this->pass.assign(pass);
            this->balance = balance;
            this->num_readers = 0;
            pthread_mutex_init(&this->r_lock,NULL);
            pthread_mutex_init(&this->w_lock,NULL);
        }
        //Account destructor.
        ~account(){
            pthread_mutex_destroy(&this->r_lock);
            pthread_mutex_destroy(&this->w_lock);
        }
};

#endif