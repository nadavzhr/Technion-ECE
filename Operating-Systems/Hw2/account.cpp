#include "account.hpp"

using namespace std;

void account::read_lock(){
    pthread_mutex_lock(&r_lock);
    this->num_readers++;
    if(this->num_readers==1){
        pthread_mutex_lock(&w_lock);
    }
    pthread_mutex_unlock(&r_lock);
}
void account::read_unlock(){
    pthread_mutex_lock(&r_lock);
    this->num_readers--;
    if(!this->num_readers){
        pthread_mutex_unlock(&w_lock);
    }
    pthread_mutex_unlock(&r_lock);
}
void account::write_lock(){
    pthread_mutex_lock(&w_lock);
}
void account::write_unlock(){
    pthread_mutex_unlock(&w_lock);
}
