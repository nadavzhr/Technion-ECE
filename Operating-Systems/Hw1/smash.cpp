/*	smash.c
main file. This file contains the main function of smash
*******************************************************************/
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include "commands.h"
#include "signals.h"
#define MAX_LINE_SIZE 80
#define MAXARGS 20

char* L_Fg_Cmd;
std::vector<job>* jobs = new std::vector<job>(); //This represents the list of jobs. Please change to a preferred type (e.g array of char*)
char curr_fg_cmd[MAX_LINE_SIZE];
char lineSize[MAX_LINE_SIZE]; 
int removed_jobID = 0;
int removed_PID = 0;

//Global parameters
int smash_pid;
int current_fg_pID;
//**************************************************************************************
// function name: main
// Description: main function of smash. get command from user and calls command functions
//**************************************************************************************
int main(int argc, char *argv[])
{
    char cmdString[MAX_LINE_SIZE]; 	 
	smash_pid = getpid();  
	current_fg_pID = smash_pid; // at first they are the same
	//signal declaretions
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	
	struct sigaction ctrlC, ctrlZ;
	ctrlC.sa_handler = &ctrl_c_handler;
	ctrlZ.sa_handler = &ctrl_z_handler;

	ctrlC.sa_flags = SA_RESTART;
	ctrlZ.sa_flags = SA_RESTART;
	/************************************/
	//NOTE: the signal handlers and the function/s that sets the handler should be found in siganls.c
	//set your signal handlers here

    sigaction(SIGINT, &ctrlC, NULL);
    sigaction(SIGTSTP, &ctrlZ, NULL);
	/************************************/

	/************************************/
	// Init globals 

	L_Fg_Cmd =(char*)malloc(sizeof(char)*(MAX_LINE_SIZE+1));
	if (L_Fg_Cmd == NULL) 
			exit(-1); 
	L_Fg_Cmd[0] = '\0';
	
    	while (1)
    	{
	 	printf("smash > ");
		fgets(lineSize, MAX_LINE_SIZE, stdin);
		strcpy(cmdString, lineSize);    	
		cmdString[strlen(lineSize)-1]='\0';
		remove_all_finished_jobs(jobs);
					// background command
	 	if(!BgCmd(lineSize, jobs)) continue; 
					// built in commands
		ExeCmd(jobs, lineSize, cmdString);
		/* initialize for next line read*/
		lineSize[0]='\0';
		cmdString[0]='\0';
	}
    return 0;
}

