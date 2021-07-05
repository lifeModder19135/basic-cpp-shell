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
#define clear() printf("\033[H\033[J") 

/*
 *To Do:
 * - fix pipe handling
 * - incorporate codemode command
 * - incorporate cmset command
 * - incorporate buildconf command
 * - incorporate topkg command
 * - more testing
 *
 *
 *
 * */

void initShell()
{
    clear();
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
            printf("\nERROR: Could not execute.");
        }
        exit(0);
    } else {
        //Wait for child to terminate
        wait(NULL);
        return;
    }
}


void execArgsPiped(char** parsed, char** parsedPipe)
{
    int pipeEnds[2];
    pid_t p1, p2;
    if (pipe(pipeEnds) < 0) {
        printf("\nERROR: Could not create pipe. Values passed may be insufficient.");
        return;
    }
    p1 = fork();
    if (p1 < 0){        
        printf("\nERROR: Could not execute forking. Values passed may be insufficient.");
        return;
    } else if(p1 == 0) {
        close(pipeEnds[0]);
        dup2(pipeEnds[1], STDOUT_FILENO); 
        close(pipeEnds[1]);

        if (execvp(parsed[0], parsed) < 0) {  
            printf("\nERROR: Could not execute first command. Values passed may be insufficient.");
            exit(0);
        }
    } else {
        p2 = fork();
        
        if (p2 < 0) { 
            printf("\nERROR: Could not execute forking. Values passed may be insufficient.");
            return;
        }

        if (p2 == 0) { 
            close(pipeEnds[1]);
            dup2(pipeEnds[0], STDIN_FILENO); 
            close(pipeEnds[0]);
            if (execvp(parsedPipe[0], parsedPipe) < 0) {
                printf("\nERROR: Could not execute second command. Values passed may be insufficient.");
                exit(0);
            }
        } else {
            wait(NULL);
            wait(NULL);
        } 
    }         
}

void displayHelpMenu() {
    puts("\n--------HELP--------"
         "\n\n\n EXPIRIMENTAL!!!!!"  
         "\nAll UNIX shell commands should work."
         "\nThis shell also currently supports pipes"  
         "\nand whitespace fixes"
         "\n");

        return;
}

int customCommandHandler(char** parsed) {
    int numberOfCustCmds = 4, i, switchArg = 0;
    char* listOfCustomCommands[numberOfCustCmds];
    char* username;

    listOfCustomCommands[0] = "exit";   
    listOfCustomCommands[1] = "cd";  
    listOfCustomCommands[2] = "help";
    listOfCustomCommands[3] = "hello";
    // listOfCustomCommands[4] = "codemode";   
    // listOfCustomCommands[5] = "cmset";  
    // listOfCustomCommands[6] = "buildconf";
    // listOfCustomCommands[7] = "topkg";
    // listOfCustomCommands[8] = "";   
    // listOfCustomCommands[9] = "";  
    // listOfCustomCommands[10] = "";
    // listOfCustomCommands[11] = "";
    
    for (i = 0; i < numberOfCustCmds; i++) {
        if (strcmp(parsed[0], listOfCustomCommands[i]) == 0) {
            switchArg = i+1;
            break;
        } 
    }

    switch (switchArg) {
        case 1:        
           printf("\nGoodbye\n"); 
           exit(0);            
        case 2:
            chdir(parsed[1]);
            return 1;
        case 3:
            displayHelpMenu();
            return 1;
        case 4:
            username = getenv("USER");
            printf("\nHello, %s. \nThis is no place to fool around!", username);
            return 1;
       //  case 5:
        default:
            break;
    }
    return 0;
}

int parsePipe(char* str, char** strpiped) 
{
    int i; 
       
    for (i = 0; i < 2; i++) {
        strpiped[i] = strsep(&str, "|");         
        if (strpiped[i] == NULL) {
            break;
        }
    }    
    if (strpiped[1] == NULL) {
       return 0;
    } else {
        return 1;
    }
}

void parseSpace(char* str, char** parsed) 
{
    int i; 
        
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");         
        if (parsed[i] == NULL)
            break;
        if (strlen(parsed[i]) == 0)
            i--;
    }
}

int processInputString(char* input, char** parsed, char** parsedpipe)
{
    char* inputpiped[2];
    int piped = 0;

    piped = parsePipe(input, inputpiped);

    if (piped) {
        parseSpace(inputpiped[0], parsed); 
        parseSpace(inputpiped[1], parsedpipe); 
    } else {
        parseSpace(input, parsed);
    }

    if (customCommandHandler(parsed))
        return 0;
    else
        return 1 + piped;
}

int main()
{
    char inputString[MAXCOM], *parsedArgs[MAXLIST];
    char* parsedArgsPiped[MAXLIST];
    int execFlag = 0;

    initShell();

    while (1) {
        printdir();
        if (takeInput(inputString))
            continue;
        execFlag = processInputString(inputString, parsedArgs, parsedArgsPiped);

        if (execFlag == 1)
            execArgs(parsedArgs); 
        if (execFlag == 2)
            execArgsPiped(parsedArgs, parsedArgsPiped);
    }
    return 0;
}    
