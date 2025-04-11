// signals.c
// contains signal handler funtions
// contains the function/s that set the signal handlers
/*******************************************/
/* Name: handler_cntlc
   Synopsis: handle the Control-C */
#include <iostream>
#include <vector>
#include <algorithm>
#include "signals.h"
#include "commands.h"

extern int smash_pid;
extern int current_fg_pID;
extern std::vector<job>* jobs;
extern char curr_fg_cmd[MAX_LINE_SIZE];
extern int removed_jobID;
extern int removed_PID;

// Custom comparator function to sort Jobs based on job_id
bool compareJobs(const job& job1, const job& job2) {
    return job1.jobID < job2.jobID;
}

// SIGINT - Interrupt signal
void ctrl_c_handler(int signal){
      // only send SIGKILL if it is a child process - used to ensure we don't kill the SMASH
      if(current_fg_pID != smash_pid){
         std::cout << "smash: caught ctrl-C" << std::endl;
         if(kill(current_fg_pID, SIGKILL) == ERROR){
            perror("smash error: kill failed");
         }
         std::cout << "smash: process " << current_fg_pID << " was killed" << std::endl;
         current_fg_pID = smash_pid;

      // fg job is the SMASH itself
      } else {
         std::cout << "smash: caught ctrl-C" << std::endl;
      }



}

// SIGTSTP - Stop signal
void ctrl_z_handler(int signal){

   // only send SIGTSTP if it is a child process - used to ensure we don't kill the SMASH
	if(current_fg_pID != smash_pid){

      std::cout << "smash: caught ctrl-Z" << std::endl;
      remove_all_finished_jobs(jobs);

      // cout << "  pid to stop is : " << current_fg_pID << endl;
      if(kill(current_fg_pID, SIGSTOP) == ERROR){
         perror("smash error: kill failed");
         return;
      }
      std::cout << "smash: process " << current_fg_pID << " was stopped" << std::endl; 


      //Check if the proccess was already in the list
      if(removed_jobID != 0 && removed_PID == current_fg_pID){
         jobs->push_back(job(removed_jobID, current_fg_pID, STOPPED, curr_fg_cmd));
         std::sort(jobs->begin(), jobs->end(), compareJobs);
      }else{
            // insert the stopped job into the list
         if(jobs->size() >= MAX_JOBS){
            std::cerr << "job list is full" << std::endl;
            return;
         }else{
            if(!jobs->empty()){
               size_t last_jobID = (jobs->back()).jobID;
               jobs->push_back(job(last_jobID+1, current_fg_pID, STOPPED, curr_fg_cmd));
            }else{
               jobs->push_back(job(1, current_fg_pID, STOPPED, curr_fg_cmd));
            }

         }
      }
         current_fg_pID = smash_pid;
         removed_jobID = 0;
      
   
   // fg job is the SMASH itself - ignore and dont quit the process
   } else {
      std::cout << "smash: caught ctrl-Z" << std::endl;
   }
}
