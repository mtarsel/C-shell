#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parse.h"   /*include declarations for parse-related structs*/
#include <unistd.h>

enum
BUILTIN_COMMANDS { NO_SUCH_BUILTIN=0, EXIT,JOBS};

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
	printf("\ncurrent working directory: %s \n", cwd);
 
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
    /*com->command tells the command name of com*/
    if (isBuiltInCommand(com->command) == EXIT){
      exit(1);
    }
  
    /*insert your code here.*/

    free_info(info);
    free(cmdLine);
  }/* while(1) */
}
  





