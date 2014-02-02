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

/*prints current working directory*/
void printCWD(){

    char cwd[256];

/*to change directory and test cwd, use this snippet:
    if(chdir("/tmp") != ){ //On success 0 is returned    
	perror("chdir() ain't workin mann");
    } */
 
    if (getcwd(cwd, sizeof(cwd)) == NULL){
	perror("getcwd() ain't workin dudeee");
    }else{
	printf("%s", cwd);
 
    }
}


void _ (parseInfo *info){
    pid_t pid;

    pid = fork();
    if(pid == 0){
	execvp(info->CommArray[0].command, info->CommArray[0].VarList);
    }else{
	wait(0);
	printf("at parent function");	
    }
}

char *
buildPrompt()
{
    printCWD(); 
    return  "%";
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

    FILE *infile, *outfile;  
    int stdIN, stdOUT;
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

    if(strcmp(info->inFile,"")!=0){
	infile = fopen(info->inFile, "r");
	dup2(fileno(infile), 0);
    }
    
    if(strcmp(info->outFile,"")!=0){

	outfile = fopen(info->outFile, "w");
	dup2(fileno(outfile), 1);
    }

    _(info);
    close(fileno(infile));
    close(fileno(outfile));

    dup2(stdIN,0);
    dup2(stdOUT,1);

/*    if (isBuiltInCommand(com->command) == HISTORY){
    
    	

    }*/

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
