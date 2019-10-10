#include <stdio.h>     // printf, fgets, stdin
#include <unistd.h>    // pipe, fork, write, read, execvp, sleep
#include <sys/types.h> // pid_t
#include <sys/wait.h>  // wait
#include <string.h>    // strlen
#include <sys/stat.h>  // mkfifo
#include <fcntl.h>     // open, O_WRONLY, O_RDONLY
#include <signal.h>    // signal
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>

#define SIZE 256
#define EXITCMD "exit"
#define CHDIRCMD "chdir"
#define CDCMD "cd"
#define HISTORYCMD "history"

void interruptHandler(int sig);
int length(char *line);
char *get_a_line();
int my_system(char *line);
int execArgsPiped(char *line);

char line[SIZE] = {0x0};
char buffer[SIZE];

char hist[100][100]; //[nber of strings][stringlength+1]
int histCount = 0;

struct rlimit rl;

// Return: 0 on exit, 1 if any failures happen
int main()
{
    signal(SIGINT, interruptHandler);
    signal(SIGTSTP, interruptHandler);
    while (1)
    {
        strcpy(line, get_a_line());
        if (length(line) > 1)
        {
            my_system(line);
        }
    }
}

// function that counts the number of characters in the line array
int length(char *s)
{
    int len = strlen(s) - 1;
    return len;
}

// function that returns a single line
char *get_a_line()
{
    // checks for end of file
    // exit if reached the end of file
    if (feof(stdin))
    {
        exit(0);
    }
    fgets(buffer, sizeof(buffer), stdin);
    return buffer;
}

int my_system(char *line)
{
    // copy current command to history array
    strcpy(hist[histCount], line);
    histCount++;

    char *s = line;
    char *args[SIZE] = {NULL};

    // converts the whole line to a list of arguments
    for (int i = 0; i < sizeof(args) && *s; s++)
    {
        if (*s == ' ')
            continue;
        if (*s == '\n')
            break;
        for (args[i++] = s; *s && *s != ' ' && *s != '\n'; s++)
            ;
        *s = '\0';
    }

    // bult-in command for change directory
    // putting it before forking, so it will change the directory in not only the child's process
    // but parent's too
    if (strcmp(CHDIRCMD, args[0]) == 0 || strcmp(CDCMD, args[0]) == 0)
    {
        if (args[1] != NULL)
        {
            if (chdir(args[1]) != 0)
            {
                perror("Changing directory failed");
            }
        }
    }
    else
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("Fork failed");
            exit(1);
        }
        if (pid == 0)
        {
            //CHILD
            // checks for end of file
            if (feof(stdin))
                exit(0);
            // built-in history command
            // prints the last 100 commands executed
            // bult-in exit command
            if (strcmp(EXITCMD, args[0]) == 0)
                exit(0);
            else if (strcmp(HISTORYCMD, args[0]) == 0)
            {
                for (int i = 0; i < histCount - 1; i++)
                {
                    printf("%s", hist[i]);
                }
                fflush(stdout);
            }
            // execute command, exit if any error
            else if (execvp(args[0], args) < 0)
            {
                perror(args[0]);
                exit(1);
            }
            exit(0);
        }
        else
        {
            //PARENT
            wait(NULL);
            // bult-in exit command
            if (strcmp(EXITCMD, args[0]) == 0)
                exit(0);
        }
    }
    return 0;
}

void interruptHandler(int sig)
{
    switch (sig)
    {
    case SIGINT:
        // ignore this signal
        signal(sig, SIG_IGN);
        // ask for confirmation to terminate
        printf("\nDo you really want to quit? [y/n] ");
        char ans = getchar();
        if (ans == 'y' || ans == 'Y')
        {
            exit(0);
        }
        else
        {
            // else, ignore signal again
            rewind(stdin);
            signal(SIGINT, SIG_IGN);
        }
        fflush(stdout);
        break;

    case SIGTSTP:
        // ignore this signal
        signal(sig, SIG_IGN);
        break;
    }
}
