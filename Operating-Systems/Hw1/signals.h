#ifndef _SIGS_H
#define _SIGS_H
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


// #define SIGKILL 9
// #define SIGTERM 15
// #define SIGCONT 18
// #define SIGSTOP 19
// #define SIGSTP 24 
// #define SIGINT 2
// #define SIGTSTP 20

void ctrl_c_handler(int signal);
void ctrl_z_handler(int signal);
#endif

