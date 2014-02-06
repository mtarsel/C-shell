#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/
#include <unistd.h>
#include <sys/types.h>

enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT, JOBS, CD, HISTORY};

char history_Count;
char history[100][20];


void display_history(){

    int i = 0;

    while(history[i][0] != '\0')
    {
	printf("%d  %s\n", i+1, *(history + i));
	i++;
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
      exit(1);
    }
    
    if (isBuiltInCommand(com->command) == JOBS){
	system("ps -aux | more");
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

    if(strncmp(com->command, "!", strlen("!")) == 0){
	char *tmp;
	tmp = strtok(com->command,"!");
	history_reference = atoi(tmp);
	if (history_reference <= 0){
	    printf("\n ERROR: invalid character:%s",tmp);
	} 
	printf("\nGot ya, %d \n", history_reference);
    }

/*Save commands into history array*/
    while(history[history_index][0] != '\0'){
	history_index++;
    }

    strcpy(history[history_index], com->VarList[0]);/*TODO*/
    history_Count++;

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
