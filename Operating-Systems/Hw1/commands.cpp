//		commands.c
//********************************************
#include "commands.h"

extern std::vector<job>* jobs;
char previous_path[MAX_LINE_SIZE] = "";
extern int current_fg_pID;
extern char curr_fg_cmd[MAX_LINE_SIZE];
extern int removed_jobID;
extern int removed_PID;

//********************************************
// function name: ExeCmd
// Description: interperts and executes built-in commands
// Parameters: pointer to jobs, command string
// Returns: 0 - success,1 - failure
//**************************************************************************************
int ExeCmd(std::vector<job>* jobs, char* lineSize, char* cmdString)
{
	char* cmd; 
	char* args[MAX_ARG];
	char pwd[MAX_LINE_SIZE];
	const char* delimiters = " \t\n";  
	int i = 0, num_arg = 0;
    	cmd = strtok(lineSize, delimiters);
	if (cmd == NULL)
		return 0; 
   	args[0] = cmd;
	for (i=1; i<MAX_ARG; i++)
	{
		args[i] = strtok(NULL, delimiters); 
		if (args[i] != NULL) 
			num_arg++; 
 
	}
/*************************************************/
// Built in Commands PLEASE NOTE NOT ALL REQUIRED
// ARE IN THIS CHAIN OF IF COMMANDS. PLEASE ADD
// MORE IF STATEMENTS AS REQUIRED
/*************************************************/
	if (!strcmp(cmd, "cd") ) 
	{	
		// invalid number of arguments
		if (num_arg > 1){
			std::cerr << "smash error: cd: too many arguments" << std::endl;
			return FAILURE;
		}

		char new_path[MAX_LINE_SIZE];
		strcpy(new_path, args[1]);

		// one step backwards
		if (!strcmp("-", new_path)){
			// no previous directory recorded
			if (!strcmp("",previous_path)) {
				std::cerr << "smash error: cd: OLDPWD not set" << std::endl;
				return FAILURE;
			}
			else {
				int retval = chdir(previous_path);
				if (retval == -1){
					perror("smash error: chdir failed");
					return FAILURE;
				}
				return SUCCESS;
			}
		// relative or absolute
		} else{
			// update previous path
			getcwd(previous_path, sizeof(previous_path));
			int retval = chdir(new_path);
			if (retval == -1){
				perror("smash error: chdir failed");
				return FAILURE;
			}
			return SUCCESS;
		}
	} 
	/*************************************************/
	else if (!strcmp(cmd, "pwd")) 
	{
		getcwd(pwd, sizeof(pwd));
		std::cout << pwd << std::endl;
		return SUCCESS;
	}
	/*************************************************/
	else if (!strcmp(cmd, "jobs")) 
	{
		if(NULL == jobs){
			return SUCCESS;
		}
		remove_all_finished_jobs(jobs);
		for (std::vector<job>::iterator it = jobs->begin() ; it != jobs->end(); it++){
			time_t elapsedTime = difftime(time(NULL),it->insertionTime);
			if(it->state == STOPPED){
				std::cout << "[" << it->jobID << "] " << it->command << " : " << it->processID << " " << elapsedTime << " (stopped)" << std::endl;
			}else if(it->state == BG){
				std::cout << "[" << it->jobID << "] " << it->command << " : " << it->processID << " " << elapsedTime << std::endl;
			}
		}
		return SUCCESS;
	}
	/*************************************************/
	else if (!strcmp(cmd, "showpid")) 
	{

		pid_t smashPID = getpid();
		std::cout << "smash pid is " << smashPID << std::endl;
		return SUCCESS;
		
	}
	/*************************************************/
	else if (!strcmp(cmd, "fg")) 
	{
		std::vector<job>::iterator it;
		// check for errors
		if (num_arg > 1){
			std::cerr << "smash error: fg: invalid arguments" << std::endl;
			return FAILURE;
		}
		else if (num_arg == 0){
			if(jobs->size() == 0){
				std::cerr << "smash error: fg: jobs list is empty" << std::endl;
				return FAILURE;
			}else{
				//job with the max id is the last one inserted.
				it = --(jobs->end());
				it->state = FG;
				size_t job_pid = it->processID;
				current_fg_pID = job_pid;
				strcpy(curr_fg_cmd, it->command);
				std::cout << it->command << " : "  << it->jobID << std::endl;
				if(kill(job_pid, SIGCONT) == ERROR){
					perror("smash error: kill failed");
					return FAILURE;
				}
				removed_jobID = it->jobID;
				removed_PID = it->processID;
				jobs->erase(it);
				if(waitpid(job_pid, NULL, WUNTRACED) == ERROR){
					perror("smash error: waitpid failed fg error");
					return FAILURE;
				}

				current_fg_pID = getpid();
				return SUCCESS;
			}
		}else{
			size_t current_job_id = std::atoi(args[1]);
			it = jobs->begin();
			while(it != jobs->end()){
				// look for the job in the list, move to fg if found.
				if(it->jobID == current_job_id){
					it->state = FG;
					size_t job_pid = it->processID;
					current_fg_pID = job_pid;
					strcpy(curr_fg_cmd, it->command);
					std::cout << it->command << " : "  << it->jobID << std::endl;
					if(kill(job_pid, SIGCONT) == ERROR){
						perror("smash error: kill failed");
						return FAILURE;
					}
					removed_jobID = it->jobID;
					removed_PID = it->processID;
					jobs->erase(it);
					if(waitpid(job_pid, NULL, WUNTRACED) == ERROR){
						perror("smash error: waitpid failed");
						return FAILURE;
					}

					current_fg_pID = getpid();
					return SUCCESS;
				}
				it++;
			}
			std::cerr << "smash error: fg: job-id " << current_job_id << " does not exist" << std::endl;
			return FAILURE;
		}

	} 
	/*************************************************/
	else if (!strcmp(cmd, "bg")) 
	{
		std::vector<job>::iterator it;
		// check for errors
		if (num_arg > 1){
			std::cerr << "smash error: bg: invalid arguments" << std::endl;
			return FAILURE;
		}
		// no arguments were given
		else if (num_arg == 0){
			it = --(jobs->end());
			// traverse the list from end to start
			while(it != jobs->begin()){
				// search for the first stopped job from the end of the list
				if(it->state == STOPPED){
					std::cout << it->command << " : "  << it->jobID << std::endl;
					it->state = BG;
					if(kill(it->processID, SIGCONT) == ERROR){
						perror("smash error: kill failed");
						return FAILURE;
					}
					return SUCCESS;
				}
				it--;
			}
			std::cerr << "smash error: bg: there are no stopped jobs to resume" << std::endl;
			return FAILURE;
		// exactly 1 argument was given - an ID of a job
		}else{
			size_t current_job_id = std::atoi(args[1]);
			// search for the job in the jobs list
			it = jobs->begin();
			while(it != jobs->end()){
				// job was found
				if(it->jobID == current_job_id){
					// job is stopped, resume it
					if(it->state == STOPPED){
						std::cout << it->command << " : "  << it->jobID << std::endl;
						it->state = BG;
						if(kill(it->processID, SIGCONT) == ERROR){
							perror("smash error: kill failed");
							return FAILURE;
						}
						return SUCCESS;
					// job is already running
					}else{
						std::cerr << "smash error: bg: job-id " << current_job_id << " is already running in the background" << std::endl;
						return FAILURE;
					}
				}
				it++;
			}
			std::cerr << "smash error: bg: job-id " << current_job_id << " does not exist" << std::endl;
			return FAILURE;

		} 
	}
	/*************************************************/
	else if (!strcmp(cmd, "quit"))
	{
		//in case of a quit command without a kill.
		if(num_arg != 1){
			exit(0);
		}
		else if(!(strcmp(args[1],"kill"))){
			vector<job>::iterator it;
			for(it = jobs->begin(); it != jobs->end(); it++){
				if(kill(it->processID, SIGTERM) == ERROR){
					perror("smash error: kill failed");
					return FAILURE;
				}
				std::cout << "[" << it->jobID << "] " << it->command << " - Sending SIGTERM...";
				std::cout.flush();
				sleep(5);
				if(waitpid(it->processID,NULL,WNOHANG) == 0){
					std::cout << " (5 sec passed) Sending SIGKILL...";
					if(kill(it->processID, SIGKILL) == ERROR){
						perror("smash error: kill failed");
						return FAILURE;
					}
					std::cout << " Done." << std::endl;
					continue;
				}
				std::cout << " Done." << std::endl;
			}

		}
		exit(0);
	} 
	/*************************************************/
	else if (!strcmp(cmd, "kill"))
	{
		if(num_arg != 2){
			std::cerr << "smash error: kill: invalid arguments" << std::endl;
			return FAILURE;
		}else{
			size_t targetJobId = std::atoi(args[2]);
			char signal_id_first[MAX_LINE_SIZE] ;
			strcpy(signal_id_first,args[1]);
			if(signal_id_first[0] != '-'){
				std::cerr << "smash error: kill: invalid arguments" << std::endl;
				return FAILURE;
			}
			//Ignore the "-" at the beginning of the argument
			char* signal_id = signal_id_first+1;
   			std::vector<job>::iterator it;
			it = jobs->begin();
			while(it != jobs->end()){
				if(it->jobID == targetJobId){
					size_t process_id = it->processID;
					if(std::atoi(signal_id) == SIGSTOP){
						it->state = STOPPED;
					}else if(std::atoi(signal_id) == SIGCONT){
						it->state = BG;
					}else if(std::atoi(signal_id) == SIGKILL || std::atoi(signal_id) == SIGTERM){
						jobs->erase(it);
					}
					if(kill(process_id, std::atoi(signal_id)) == ERROR){
						perror("smash error: kill failed");
						return FAILURE;
					}else{
						std::cout << "signal number " << signal_id << " was sent to pid " <<  process_id << std::endl;
						return SUCCESS;
					} 
				}else{
					it++;
				}
			}
			if(it == jobs->end()){
				std::cerr << "smash error: kill: job-id " << targetJobId << " does not exist" << std::endl;
				return FAILURE;
			}
		}
		
	} 
	/*************************************************/	
	else if (!strcmp(cmd, "diff"))
	{	// CHECK FOR THE CORRECTNESS OF THE FUNCTION
        if(num_arg != 2){
            std::cerr << "smash error: diff: invalid arguments" << std::endl;
			return FAILURE;
        }

        if (!strcmp(args[1], args[2])) {
            std::cout << "0" << std::endl;
            return SUCCESS;
        }

        else {
            int file1;
            int file2;

            if ((file1 = open(args[1], O_RDONLY)) == -1) {
                perror("smash error: open failed");
                return FAILURE;
            }

            if ((file2 = open(args[2], O_RDONLY)) == -1) {
                perror("smash error: open failed");
                return FAILURE;
            }

            FILE *fp1 = fopen(args[1], "r");
            FILE *fp2 = fopen(args[2], "r");
			if (fp1 == NULL || fp2 == NULL){
				perror("smash error: open failed");
				return FAILURE;
			}
            char ch1 = getc(fp1);
            char ch2 = getc(fp2);

            while (ch1 != EOF && ch2 != EOF) {
                if (ch1 != ch2) {
                    std::cout << "1" << std::endl;
					return SUCCESS;
                }

                ch1 = getc(fp1);
                ch2 = getc(fp2);
            }
            std::cout << "0" << std::endl;
			return SUCCESS;
        }

    }
	
	/*************************************************/	
	else // external command
	{
		//not a bg command
 		ExeExternal(args, cmdString, jobs, false);
	 	return 0;
	}

    return 0;
}

//**************************************************************************************
// function name: ExeExternal
// Description: executes external command
// Parameters: external command arguments, external command string
// Returns: void
//**************************************************************************************
void ExeExternal(char *args[MAX_ARG], char* cmdString, std::vector<job>* jobs , bool is_bg)
{
	int pID;
	strcpy(curr_fg_cmd,cmdString);
    	switch(pID = fork()) 
		{
    		case -1: 
				perror("smash error: fork failed");
				exit(1);

        	case 0 :
                	// Child Process
               		setpgrp();
              		execvp(args[0], args);
               		perror("smash error: exec failed");
					exit(1);
			// father process
			default:
					if(is_bg){
						// Assuming jobs is a pointer to a non-empty vector<job>
						remove_all_finished_jobs(jobs);
						if(jobs->size() >= MAX_JOBS){
							std::cerr << "job list is full" << std::endl;
						}else{
							if(!jobs->empty()){
								size_t last_jobID = (jobs->back()).jobID;
								jobs->push_back(job(last_jobID+1, pID, BG, cmdString));
							}else{
								jobs->push_back(job(1, pID, BG, cmdString));
							}
						}
					// run on foreground
					}else{
						current_fg_pID = pID;
						// waits for the child process to finish.
						if (waitpid(pID, NULL, WUNTRACED | WCONTINUED) == ERROR){
							perror("smash error: waitpid failed");
						}		
						// restore the smash pid to be the father pid				
						current_fg_pID = getpid();
					}
		}
}

//**************************************************************************************
// function name: BgCmd
// Description: if command is in background, insert the command to jobs
// Parameters: command string, pointer to jobs
// Returns: 0- BG command -1- if not
//**************************************************************************************
int BgCmd(char* lineSize, std::vector<job>* jobs)
{
	char* cmd;
	const char* delimiters = " \t\n";
	char *args[MAX_ARG];


	if (lineSize[strlen(lineSize)-2] == '&')
	{
		char cmdName[MAX_LINE_SIZE];
		strcpy(cmdName, lineSize);
		//cmdName will hold the name including the '&'
		cmdName[strlen(cmdName)-1] = '\0';
		// extract command and arguments.
		lineSize[strlen(lineSize)-2] = '\0';
		int i = 0;
	    cmd = strtok(lineSize, delimiters);
		if (cmd == NULL)
			return 0;
	   	args[0] = cmd;
		//fill args array.
		for (i=1; i<MAX_ARG; i++)
		{
			args[i] = strtok(NULL, delimiters);
		}
		// ignore built-in commands with &
		if(is_built_in_cmd(args[0])){
			return -1;
		}else{
			ExeExternal(args, cmdName, jobs, true);
		}

		return 0;
	}
	return -1;
}
void remove_all_finished_jobs(std::vector<job>* job_list){
	if(job_list == NULL){
		return;
	}
	if(job_list->size() == 0){
		return;
	}
	std::vector<job>::iterator it = job_list->begin();
	while (it != job_list->end() ){
		//If the process has terminated
		int retval = waitpid(it->processID,NULL,WNOHANG);
		if(retval == ERROR){
			perror("smash error: waitpid failed");
			return;
		}else if(retval != 0){
			it = job_list->erase(it); //job is finished, now 'it' pointes to the next element
		}else{
			it++;
		}	
	}
}

bool is_built_in_cmd(char* cmd){

	return (!strcmp(cmd, "cd") || !strcmp(cmd, "kill") || !strcmp(cmd, "diff") || 
			!strcmp(cmd, "fg") || !strcmp(cmd, "bg") || !strcmp(cmd, "showpid") || !strcmp(cmd, "pwd") || 
			!strcmp(cmd, "jobs") || !strcmp(cmd, "quit"));
}
