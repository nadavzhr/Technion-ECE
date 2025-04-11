
#include <iostream>
#include <vector>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <sstream>
#include <fstream>
#include "account.hpp"
#include "atm.hpp"

using namespace std;

extern vector<account>* all_accounts;
extern ofstream log_file;
// extern pthread_mutex_t global_lock;
extern pthread_mutex_t log_lock;

account* get_account(vector<account>* all_accounts, int account_id);

void atm::open_func(int account_id, string pass, int initial_amount){
    //pthread_mutex_lock(&global_lock);
    this->my_bank->bank_w_start();

    if(get_account(all_accounts,account_id) != NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account with the same id exists" << endl; 
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_w_end();

        return;
    }
    account* new_account = new account(account_id, pass, initial_amount);
    all_accounts->push_back(*new_account);
    pthread_mutex_lock(&log_lock);
    log_file<<this->id<<": New account id is " << account_id << " with password " << pass << " and initial balance " << initial_amount << endl;
    pthread_mutex_unlock(&log_lock);
    //pthread_mutex_unlock(&global_lock);

    this->my_bank->bank_w_end();



}

void atm::deposit_func(int account_id, string pass, int amount){
    this->my_bank->bank_r_start();

    //pthread_mutex_lock(&global_lock);
    account* curr = get_account(all_accounts,account_id);
    // account does not exist
    if(curr == NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_r_end();


        return; 
    }
    int new_balance;
    //check for correct password
    if(!curr->pass.compare(pass)){
        curr->write_lock();
        // perform the deposit
        new_balance = curr->balance + amount;
        curr->balance = new_balance;
        curr->write_unlock();
        // log the action
        pthread_mutex_lock(&log_lock);
        log_file<<this->id<<": Account " << account_id<<" new balance is "<<new_balance<<" after "<<amount<<" $ was deposited"<<endl;
        pthread_mutex_unlock(&log_lock);

    // incorrect password
    }else{
        pthread_mutex_lock(&log_lock);
        log_file<<"Error " << this->id<<": Your transaction failed - password for account id "<< account_id<< " is incorrect"<<endl;
        pthread_mutex_unlock(&log_lock);
    }
    //pthread_mutex_unlock(&global_lock);
    this->my_bank->bank_r_end();


}
void atm::withdraw_func(int account_id, string pass, int amount){
    this->my_bank->bank_r_start();

    //pthread_mutex_lock(&global_lock);
    account* curr = get_account(all_accounts,account_id);
    // account does not exist
    if(curr == NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_r_end();
        return; 
    }
    int prev_balance;
    int new_balance;

    //check for correct password
    if(!curr->pass.compare(pass)){
        curr->read_lock();
        prev_balance = curr->balance;
        curr->read_unlock();

        // check for a valid withdrawal
        if (prev_balance < amount){
            pthread_mutex_lock(&log_lock);
            log_file << "Error " << this->id << ": Your transaction failed - account id " << account_id << " balance is lower than " << amount << endl;
            pthread_mutex_unlock(&log_lock);
            //pthread_mutex_unlock(&global_lock);
            this->my_bank->bank_r_end();
            return;
        }
        // perform the withdrawal
        curr->write_lock();
        new_balance = prev_balance - amount;
        curr->balance = new_balance;
        curr->write_unlock();
        // log the action
        pthread_mutex_lock(&log_lock);
        log_file<<this->id<<": Account " << account_id<<" new balance is "<<new_balance<<" after "<<amount<<" $ was withdrew"<<endl;
        pthread_mutex_unlock(&log_lock);

    // incorrect password
    }else{
        pthread_mutex_lock(&log_lock);
        log_file<<"Error " << this->id<<": Your transaction failed - password for account id "<< account_id<< " is incorrect"<<endl;
        pthread_mutex_unlock(&log_lock);
    }
    //pthread_mutex_unlock(&global_lock);
    this->my_bank->bank_r_end();


    
}

void atm::balance_func(int account_id, string pass){
    this->my_bank->bank_r_start();
    //pthread_mutex_lock(&global_lock);
    account* curr = get_account(all_accounts,account_id);
    // account does not exist
    if(curr == NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account id " << account_id << " does not exist" << endl;
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_r_end();

        return; 
    }
    //check for correct password
    if(!curr->pass.compare(pass)){
        curr->read_lock();
        int tmp_id = curr->id;
        int tmp_balance = curr->balance;
        curr->read_unlock();
        pthread_mutex_lock(&log_lock);
        log_file << this->id<<": Account " << tmp_id <<" balance is " << tmp_balance <<  endl;
        pthread_mutex_unlock(&log_lock);

    // incorrect password
    }else{
        pthread_mutex_lock(&log_lock);
        log_file<<"Error " << this->id<<": Your transaction failed - password for account id "<< account_id<< " is incorrect"<<endl;
        pthread_mutex_unlock(&log_lock);
    }
    //pthread_mutex_unlock(&global_lock);
    this->my_bank->bank_r_end();

}

void atm::close_func(int account_id, string pass){

    this->my_bank->bank_w_start();
    //pthread_mutex_lock(&global_lock);

    vector<account>::iterator it = all_accounts->begin();
    while (it != all_accounts->end()){
        if (it->id == account_id){
            if (!it->pass.compare(pass)){
                it->read_lock();
                int tmp_balance = it->balance;
                it->read_unlock();
                // log the action
                pthread_mutex_lock(&log_lock);
                log_file << this->id << ": Account " << account_id << " is now closed. Balance was " << tmp_balance << endl;
                pthread_mutex_unlock(&log_lock);
                // remove the account from the bank
                all_accounts->erase(it);
                //pthread_mutex_unlock(&global_lock);
                this->my_bank->bank_w_end();

                return;
            } else {
                pthread_mutex_lock(&log_lock);
                log_file<<"Error " << this->id<<": Your transaction failed - password for account id "<< account_id<< " is incorrect"<<endl;
                pthread_mutex_unlock(&log_lock);
                //pthread_mutex_unlock(&global_lock);
                this->my_bank->bank_w_end();
                return;
            }
        }
        it++;
    }
    // account was not found in the bank
    pthread_mutex_lock(&log_lock);
    log_file << "Error " << this->id << ": Your transaction failed - account id " << account_id << " does not exist" << endl;
    pthread_mutex_unlock(&log_lock);
    //pthread_mutex_unlock(&global_lock);
    this->my_bank->bank_w_end();

    return; 
}

void atm::transfer_func(int src_account, string pass, int target_account, int amount){
    this->my_bank->bank_r_start();

    //pthread_mutex_lock(&global_lock);

    // check for the presence of the accounts
    account* src = get_account(all_accounts,src_account);
    // src account does not exist
    if(src == NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account id " << src_account << " does not exist" << endl;
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_r_end();

        return; 
    }
    account* target = get_account(all_accounts,target_account);
    // target account does not exist
    if(target == NULL){
        pthread_mutex_lock(&log_lock);
        log_file <<"Error "<< this->id <<": Your transaction failed - account id " << target_account << " does not exist" << endl;
        pthread_mutex_unlock(&log_lock);
        //pthread_mutex_unlock(&global_lock);
        this->my_bank->bank_r_end();

        return; 
    }
    //Lock the writers in the same order every time
    if (target->id < src->id){
        target->write_lock();
        src->write_lock();
    }
    else{
        src->write_lock();
        target->write_lock();

    }

    // both accounts exist - perform the transfer
    int prev_balance;
    int new_target_balance;
    //check for correct password
    if(!src->pass.compare(pass)){
        prev_balance = src->balance;
        // check for a valid withdrawal
        if (prev_balance < amount){
            pthread_mutex_lock(&log_lock);
            log_file << "Error " << this->id << ": Your transaction failed - account id " << src_account << " balance is lower than " << amount << endl;
            pthread_mutex_unlock(&log_lock);
            //pthread_mutex_unlock(&global_lock);
            if (target->id < src->id){
                src->write_unlock();
                target->write_unlock();

            }
            else{
                target->write_unlock();
                src->write_unlock();
            }
            this->my_bank->bank_r_end();
            return;
        }
        // src has enough money to transfer
        // src->read_unlock();
        src->balance -= amount;
        // transfer the money to the target
        // log_file << "BEFORE target R_LOCK  "<< endl;
        // log_file << "AFTER target R_LOCK  "<< endl;

        new_target_balance = target->balance + amount;
        target->balance = new_target_balance;

        pthread_mutex_lock(&log_lock);
        log_file << this->id << ": Transfer " << amount <<" from account " << src_account << \
        " to account " << target_account << " new account balance is " << prev_balance-amount << \
        " new target account balance is " << new_target_balance << endl;
        pthread_mutex_unlock(&log_lock);

    // incorrect password
    }else{
        pthread_mutex_lock(&log_lock);
        log_file << "Error " << this->id<<": Your transaction failed - password for account id "<< src_account<< " is incorrect"<<endl;
        pthread_mutex_unlock(&log_lock);
    }
    //pthread_mutex_unlock(&global_lock);
    if (target->id < src->id){
        src->write_unlock();
        target->write_unlock();

    }
    else{
        target->write_unlock();
        src->write_unlock();
    }
    this->my_bank->bank_r_end();


}

account* get_account(vector<account>* all_accounts, int account_id){

    vector<account>::iterator it = all_accounts->begin();
    while (it != all_accounts->end()){
        // account is found in the bank
        if (it->id == account_id){
            return &(*it);
            break;
        }
        it++;

    }
    // account was not found in the bank
    return NULL;
}

void atm::action_handler(){

    ifstream atm_actions(this->atm_actions_file.c_str());

    if (!atm_actions.is_open()){
        cerr << "Bank error: illegal arguments" << endl;
        atm_actions.close();
        exit(1);
    }

    char action;
    // this string holds a single line in the input txt file - containing an action and its arguments
    string action_line;
    int acc_id;
    string password;

    while(getline(atm_actions, action_line)) {
        if (action_line.empty()){
            continue;
        }
        // extract arguments from the line
        istringstream action_stream(action_line);
        action_stream >> action >> acc_id >> password;
        int amount;
        switch(action) {
            case 'O':
                action_stream >> amount;
                this->open_func(acc_id, password, amount);
                sleep(ATM_ACT_TIME);
                break;
            case 'D':
                action_stream >> amount;
                this->deposit_func(acc_id, password, amount);
                sleep(ATM_ACT_TIME);
                break;
            case 'W':
                action_stream >> amount;
                this->withdraw_func(acc_id, password, amount);
                sleep(ATM_ACT_TIME);
                break;
            case 'B':
                this->balance_func(acc_id, password);
                sleep(ATM_ACT_TIME);
                break;
            case 'Q':
                this->close_func(acc_id, password);
                sleep(ATM_ACT_TIME);
                break;
            case 'T':
                int target_account_id;
                action_stream >> target_account_id >> amount;
                this->transfer_func(acc_id, password, target_account_id, amount);
                sleep(ATM_ACT_TIME);
                break;
            
            default:
                break;
        }
        // Sleep for 0.1sec after execution of a command
        usleep(ATM_SLEEP_TIME);
    }
    // close the txt file after executing all the commands
    atm_actions.close();

}
