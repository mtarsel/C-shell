#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>


enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT, JOBS, CD, HISTORY, KILL};

char Global_history_count;
char Global_jobs_count;
char history[100][20];
char jobs[100][20];


void display_history(){

    int i = 0;

    while(history[i][0] != '\0'){
	printf("%d  %s\n", i+1, *(history + i));
	++i;
    }

}

void display_jobs(){
    int i = 0;
   
    if(jobs[i][0] != '\0'){
	printf("\n fuckin made it \n");
	printf("%d %s\n", i+1, *(jobs + i));
	++i;
    }

}


void makebackgroundjobs(){
    
    char  *pArgs[10];
    int background;
    pid_t pid;
    int status;

    if (strlen(pArgs[0]) > 1) {
	pid = fork();
	if (pid == -1) {
	    perror("fork");
	    exit(1);
	} else if (pid == 0) {
	    execvp(pArgs[0], pArgs);
	    exit(1);
	} else if (!background) {
	    pid = waitpid(pid, &status, 0);
	    if (pid > 0)
		printf("waitpid reaped child pid %d\n", pid);
	}
    }


}


/*prints current working directory*/
void printPrompt(){

    char cwd[256];
    char *username=getenv("USER"); 
    
    char hostname[1024];
    
    hostname[1023] = '\0';
    gethostname(hostname, 1023);
/*to change directory and test cwd, use this snippet:
    if(chdir("/tmp") != ){ //On success 0 is returned    
	perror("chdir() ain't workin mann");
    } */
 
    if (getcwd(cwd, sizeof(cwd)) == NULL){
	perror("getcwd() ain't workin dudeee");
    }else{
	printf("%s@%s %s ", username, hostname, cwd);
 
    }
}


void _ (parseInfo *info){
    pid_t pid;
    int status = 1;

    pid = fork();
    if(pid == 0){
	execvp(info->CommArray[0].command, info->CommArray[0].VarList);
    }else{
	waitpid(-1, &status, 0);
/*	wait(pid);
	printf("at parent function");*/
    }
}

char *
buildPrompt()
{
    printPrompt();
    return  "$";
}
 
int
isBuiltInCommand(char * cmd){
  
  if ( strncmp(cmd, "exit", strlen("exit")) == 0){
    return EXIT;
  }
  if ( strncmp(cmd, "jobs", strlen("jobs")) == 0){
    return JOBS;
  }
  if ( strncmp(cmd, "cd", strlen("cd")) == 0){
    return CD;
  }
  if ( strncmp(cmd, "kill", strlen("kill")) == 0){
    return KILL;
  }
  if ( strncmp(cmd, "history", strlen("history")) == 0){
    return HISTORY;
  }
  return NO_SUCH_BUILTIN;
}


int 
main (int argc, char **argv)
{

    FILE *infile = NULL;
    FILE *outfile = NULL;  
    int stdIN, stdOUT;
    int history_index = 0;
    int jobs_index = 0;
    int history_reference = 0;
    char * cmdLine;
    parseInfo *info; /*info stores all the information returned by parser.*/
    struct commandType *com; /*com stores command name and Arg list for one command.*/
    
    stdIN = dup(fileno(stdin));
    stdOUT = dup(fileno(stdout));
    
#ifdef UNIX
    fprintf(stdout, "This is the UNIX version\n");
#endif

#ifdef WINDOWS
    fprintf(stdout, "This is the WINDOWS version\n");
#endif

  while(1){
    /*insert your code to print prompt here*/

#ifdef UNIX
    cmdLine = readline(buildPrompt());
    if (cmdLine == NULL) {
      fprintf(stderr, "Unable to read command\n");
      continue;
    }
#endif

    /*insert your code about history and !x !-x here*/

    /*calls the parser*/
    info = parse(cmdLine);
    if (info == NULL){
      free(cmdLine);
      continue;
    }
    /*prints the info struct*/
    print_info(info);

    /*com contains the info. of the command before the first "|"*/
    com=&info->CommArray[0];
    if ((com == NULL)  || (com->command == NULL)) {
      free_info(info);
      free(cmdLine);
      continue;
    }



    /*com->command tells the command name of com*/
    if (isBuiltInCommand(com->command) == EXIT){
	fflush(stdout);
	fflush(stdin);
	exit(1);
    }
    
    if (isBuiltInCommand(com->command) == JOBS){
	display_jobs();
    }
    
    if (isBuiltInCommand(com->command) == CD){
	const char * const path = com->VarList[1];

	if (chdir(path) == -1){
	    printf("chdir failed\n");
	}
    }
    
    if (isBuiltInCommand(com->command) == HISTORY){
	display_history();
    }


    if (isBuiltInCommand(com->command) == KILL){
	
	const char * const path = com->VarList[1];
	int kill_pid = 0;

	kill_pid = atoi(com->VarList[1]);
	
	if(kill_pid <= 0){
	    printf("\nInvalid pid:%d\n", kill_pid);
	}else{
	    kill(kill_pid, SIGKILL);
	}
    
    }


/*check if command has a ! to check history. Also implemenets 
error checking*/
    if(strncmp(com->command, "!", strlen("!")) == 0){
	char *tmp;
	tmp = strtok(com->command,"!");
	history_reference = atoi(tmp);
	if (history_reference <= 0){
	    printf("\n ERROR: invalid character:%s",tmp);
	}

/*	cmdLine = history[history_reference-1]; TODO*/
    

	printf("\nCommand: %s \n", history[history_reference-1]);
    }


    /*check if command contains & anywhere */
    if(strncmp(com->command, "&", strlen("&")) == 0){

	printf("com->command:%s", com->command);
	
	/*Save commands into jobs array if contains &*/
	strcpy(jobs[jobs_index], com->command);
	++Global_jobs_count;
	
	makebackgroundjobs();
    }


    while(history[history_index][0] != '\0'){
	history_index++;
	
	/*if(strncmp(com->command, "&", strlen("&")) == 0){
	    strcpy(jobs[jobs_index], com->command);
	    ++Global_jobs_count;
	} */   


    }

    /*Save commands into history array*/
    strcpy(history[history_index], com->command);/*TODO only saves first arg*/
    Global_history_count++;

    if(strcmp(info->inFile,"")!=0){
	infile = fopen(info->inFile, "r");
	dup2(fileno(infile), 0);
    }
    
    if(strcmp(info->outFile,"")!=0){

	outfile = fopen(info->outFile, "w");
	dup2(fileno(outfile), 1);
    }

    _(info);
    fflush(stdout);
    fflush(stdin);
    
    if (infile != NULL){
	fclose(infile);
    }
    
    if (outfile != NULL){
	fclose(outfile);
    }

    dup2(stdIN,0);
    dup2(stdOUT,1);

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
