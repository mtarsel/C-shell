#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/
#include <unistd.h>

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

  
  char * cmdLine;
  parseInfo *info; /*info stores all the information returned by parser.*/
  struct commandType *com; /*com stores command name and Arg list for one command.*/

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


    execvp(com->command, com->VarList);

    /*com->command tells the command name of com*/
    if (isBuiltInCommand(com->command) == EXIT){
      exit(1);
    }
    
    if (isBuiltInCommand(com->command) == JOBS){
	system("ps -aux | more");
    }
    
    if (isBuiltInCommand(com->command) == CD){
	const char * const path = argv[1];
	if (chdir(path) == -1){
	    printf("chdir failed\n");
	    system("pwd");
	}else{
	    printf("\n made it here \n");
	    system("pwd");
	}
    }

    if (isBuiltInCommand(com->command) == HISTORY){
	system("history | more");
    }
    /*insert your code here.*/

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
