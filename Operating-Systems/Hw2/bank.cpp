#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string> 
#include <fstream>
#include <sstream> 
#include <algorithm>
#include <unistd.h>
#include "account.hpp"
#include "atm.hpp"
#include "bank.hpp"
using namespace std;


bool compareIDs(const account& acc1, const account& acc2);
void* tax_thread_runner(void* curr_bank);
void* snapshot_thread_runner(void* curr_bank);
void* action_handler_runner(void* atm);

ofstream log_file;
pthread_mutex_t log_lock;

// pthread_mutex_t global_lock;


vector<account>* all_accounts = new vector<account>();
vector<atm>* all_atms = new vector<atm>();

bool all_atms_done = false;
bank* new_bank = new bank(all_accounts);

int main(int argc, char* argv[]){
    //ERROR CHECK
    if(argc<2){
        cerr << "Bank error: illegal arguments" << endl;
        exit(1);
    }
    pthread_mutex_init(&log_lock,NULL);
    log_file.open("log.txt",ios::out);
    int atm_count = argc - 1;
    pthread_t* all_threads = new pthread_t[atm_count];
    for(int i=1; i<=atm_count; i++){
        string file_name = argv[i];
        atm* curr_atm = new atm(i,file_name,new_bank);
        all_atms->push_back(*curr_atm);
        //Initializing a thread for every atm.
        if(pthread_create(&all_threads[i-1], NULL, action_handler_runner, (void*)curr_atm)){
            perror("Bank error: pthread_create failed");
        }
    }
    
    

    //Tax collector thread
    pthread_t tax_thread;
    if(pthread_create(&tax_thread, NULL, tax_thread_runner, (void*)new_bank)){
        perror("Bank error: pthread_create failed");
    }

    //Snapshot thread
    pthread_t snapshot_thread;
    if(pthread_create(&snapshot_thread, NULL,snapshot_thread_runner, (void*)new_bank)){
        perror("Bank error: pthread_create failed");
    }

    //Wait for all atm threads to finish
    for(int i=0; i<atm_count; i++){
        int retval = pthread_join(all_threads[i],NULL);
        if(retval){
            perror("Error: pthread_join failed");
        }
    } 

    all_atms_done = true;

    if (pthread_join(tax_thread, NULL)) {
    	perror("Error: pthread_join failed");
    }
    if (pthread_join(snapshot_thread, NULL)) {
        perror("Error: pthread_join failed");
    }
    
    log_file.close();
    delete[] all_threads;
    delete all_accounts;
    delete new_bank;
    return 0;
}

// Custom comparator function to sort account based on account_id
bool compareIDs(const account& acc1, const account& acc2){
    return acc1.id < acc2.id;
}


void* tax_thread_runner(void* curr_bank){
    bank* bank1 = static_cast<bank*>(curr_bank);
    while(1){
        bank1->tax_collector();
        sleep(TAX_SLEEP_TIME);
        //If all atm's finished their job, we can stop.
        if(all_atms_done){
            pthread_exit(NULL);
            break;
        }
    }
    delete bank1;
}

void* snapshot_thread_runner(void* curr_bank){
    bank* bank1 = static_cast<bank*>(curr_bank);
    while(1){
        bank1->snapshot();
        usleep(SNAPSHOT_SLEEP_TIME);
        //If all atm's finished their job, we can stop.
        if(all_atms_done){
            pthread_exit(NULL);
            break;
        }
    }
    delete bank1;
}

void* action_handler_runner(void* curr_atm){
    atm* atm1 = static_cast<atm*>(curr_atm);
    atm1->action_handler();
    // end the atm thread
    delete atm1;
    pthread_exit(NULL);
}

void bank::tax_collector(){
    //pthread_mutex_lock(&global_lock);
    this->bank_w_start();

    srand(static_cast<unsigned int>(time(nullptr)));
    // Generate a random number between 1 and 5
    int random_number = (rand() % 5) + 1;
    int tax_amount = 0;
    int account_id = 0;
    vector<account>::iterator it = this->all_accounts->begin();
    std::stringstream log_buffer; // Buffer to store log text
    while(it != this->all_accounts->end()){
        it->read_lock();
        tax_amount = static_cast<int>(it->balance*(random_number))/100.0;
        account_id = it->id;
        it->balance -= tax_amount;
        it->read_unlock();
        //Lock for the bank readers and writers
        this->bank_balance += tax_amount;

                 // Append log text to buffer
        log_buffer << "Bank: commissions of " << random_number << " % were charged, the bank gained " << tax_amount << " $ from account " << account_id << endl;
        it++;
    }
    // Write buffered log text to log file
    pthread_mutex_lock(&log_lock);
    log_file << log_buffer.str();
    pthread_mutex_unlock(&log_lock);
    //pthread_mutex_unlock(&global_lock);
    this->bank_w_end();


}

void bank::snapshot(){
    //pthread_mutex_lock(&global_lock);
    this->bank_r_start();
    printf("\033[2J");
    printf("\033[1;1H");
    cout << "Current Bank Status" << endl;
    int balance;
    int account_id;
    string pass;
    sort(all_accounts->begin(), all_accounts->end(), compareIDs);
    vector<account>::iterator it = all_accounts->begin();
    while(it != all_accounts->end()){
        it->read_lock();
        balance = it->balance;
        pass.assign(it->pass);
        account_id = it->id;
        it->read_unlock();
        cout << "Account " << account_id << ": Balance - " << balance << " $, Account Password - " << pass << endl;
        it++;
    }

    cout << "The Bank has " << this->bank_balance << " $" << endl;
    this->bank_r_end();

    //pthread_mutex_unlock(&global_lock);

}

void bank::bank_r_start() {
    pthread_mutex_lock(&bank_lock); 
    while (curr_writing || waiting_writers > 0) { 
        pthread_cond_wait(&read_condition, &bank_lock); 
    }
    num_readers++; 
    pthread_cond_broadcast(&read_condition); 
    pthread_mutex_unlock(&bank_lock); 
}
void bank::bank_w_start() {
    pthread_mutex_lock(&bank_lock); 
    waiting_writers++; 
    while (num_readers > 0 || curr_writing) { 
        pthread_cond_wait(&write_condition, &bank_lock);
    }
    curr_writing = true; 
    waiting_writers--; 
    pthread_mutex_unlock(&bank_lock); 
}


void bank::bank_r_end() {
    pthread_mutex_lock(&bank_lock); 
    num_readers--; 
    if (num_readers == 0) { 
        pthread_cond_signal(&write_condition);
    }
    pthread_mutex_unlock(&bank_lock); 
}
void bank::bank_w_end() {
    pthread_mutex_lock(&bank_lock); 
    curr_writing = false; 
    pthread_cond_broadcast(&read_condition); 
    pthread_cond_signal(&write_condition); 
    pthread_mutex_unlock(&bank_lock); 
}