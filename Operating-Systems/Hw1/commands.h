#ifndef _COMMANDS_H
#define _COMMANDS_H
#include <vector>
#include <unistd.h> 
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <iostream>
#include <string>
#include <cstdlib>
#include <fcntl.h>

#define MAX_LINE_SIZE 80
#define MAX_ARG 20
#define MAX_JOBS 100

#define ERROR -1

using namespace std;

// typedef enum { FALSE , TRUE } bool; - causing errors
typedef enum { FG = 1, BG = 2, STOPPED = 3 } jobState;
enum {SUCCESS , FAILURE};

class job {
    public:
        size_t jobID; // max{current jobs in list} + 1
        size_t processID; // process ID
        jobState state; // fg / bg / stopped
        time_t insertionTime;   // time the job was inserted into the jobs list
        char command[MAX_LINE_SIZE];

        // constructor with parameters
        job(size_t job_id, size_t process_id, jobState status,char* cmd) {
            jobID = job_id;
            processID = process_id;
            state = status;
            insertionTime = time(NULL);
            strcpy(command,cmd);
        }
};


int BgCmd(char* lineSize, std::vector<job>* jobs);
int ExeCmd(std::vector<job>* jobs, char* lineSize, char* cmdString);
void ExeExternal(char *args[MAX_ARG], char* cmdString, std::vector<job>* jobs, bool is_bg);

void remove_all_finished_jobs(std::vector<job>* job_list);
bool is_built_in_cmd(char* cmd);
#endif

