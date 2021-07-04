#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>


#define MAXCOM 1000 // max number of letters to be supported
#define MAXLIST 100 // max number of commands to be supported
#define clear printf("\033[H\033[J") 
void initShell()
{
    printf("\n\n\n\n**************************************");
    printf("\n\n\n\n**************************************");
    printf("\n\n\n\n**********  HELLO WOOORLD!  **********");
    printf("\n\n\n\n**************************************");    
    char* user = getenv("USER");
    printf("\n\n\n\n**************************************");
    printf("\n\n\n\n**************************************");
    sleep(1);
    clear();
}

// take user input from command line
int takeInput(char* str)
{
    char* buffer;

    buffer = readline("\n$$$ ");
    if(strlen(buffer) != 0){
        add_history(buffer);
        strcpy(str, buffer);
        return 0;
    } else {
        return 1;
    }
}

//Prints the current working directory
void printdir()
{
    char cwd[1024];
    getcwd(cwd, sizeof(cwd));
    printf("\n Dir: %s", cwd);
}    

//Executes system commands, supplied as args, in a child process
void execArgs(char** parsed)
{
    pid_t pid = fork();
    if (pid == -1){
        printf("\nFailed forking child process.");
        return;
    } else if (pid == 0) {
        if (execvp(parsed[0], parsed) < 0) {
            printf("Error: Could not execute.")
        }
        exit(0);
    } else {
        //Wait for child to terminate
        wait(NULL);
        return;
    }
}


