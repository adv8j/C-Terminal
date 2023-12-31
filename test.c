#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdbool.h>
#include <dirent.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <ctype.h>
#include <regex.h>
#include <fcntl.h>
#define MAX 1000
int ls(char *command);
int ls_l(char *command);
int ls_A(char *command);
int ls_g(char *command);
int ls_i(char *command);
int ls_a(char *command);
int ps(char *command);
int mv(char *command);
int cp(char *command);
int grep(char *command);
void trim(char *s);
void display_help(char *command);
int main()
{
    char displayname[3 * MAX];
    char *username[MAX];
    *username = getlogin();
    char hostname[MAX];
    gethostname(hostname, MAX);
    char cwd[MAX];
    while (1)
    {
        getcwd(cwd, sizeof(cwd));
        for (int i = 0; i < 3 * MAX; i++)
            displayname[i] = '\0';
        if (strncmp(cwd, "/home/advait", strlen("/home/advait")) == 0)
        {
            cwd[0] = '~';
            for (int i = 1; i < strlen(cwd) - strlen("/home/advait") + 1; i++)
                cwd[i] = cwd[strlen("/home/advait") + i - 1];
            for (int i = strlen(cwd) - strlen("/home/advait") + 1; i < MAX; i++)
                cwd[i] = '\0';
        }
        strcat(displayname, "\001\e[00;32m\e[1m\002");
        strcat(displayname, *username);
        strcat(displayname, "@");
        strcat(displayname, hostname);
        strcat(displayname, "\001\e[0m\002:\001\e[00;34m\e[1m\002");
        strcat(displayname, cwd);
        strcat(displayname, "\001\e[0m\002$ ");
        char *command;
        command = readline(displayname);
        trim(command);
        add_history(command);
        if (strncmp(command, "exit", 4) == 0 && strlen(command) == 4)
            break;
        else if (strncmp(command, "cd", 2) == 0)
        {
            char temp[MAX];
            if (strlen(command) == 3)
            {
                printf("Invalid command\n");
                continue;
            }
            if (command[2] != ' ')
            {
                printf("Invalid command\n");
                continue;
            }

            for (int i = 0; i < strlen(command) - 3; i++)
                temp[i] = command[i + 3];
            temp[strlen(command) - 3] = '\0';
            trim(temp);
            if (chdir(temp) != 0)
                printf("Could not reach the specified location\n");
        }
        else if (strncmp(command, "ls", 2) == 0)
        {
            if (strlen(command) == 2)
            {
                char tempCWD[MAX];
                getcwd(tempCWD, sizeof(tempCWD));
                ls(tempCWD);
                continue;
            }
            command += 2;
            if (*command != ' ')
            {
                printf("Invalid ls command\n");
                continue;
            }
            *(command - 1) = '\0';
            trim(command);
            if (*command != '-')
            {
                printf("Invalid ls command\n");
                continue;
            }
            command++;
            char tempCWD[MAX];
            getcwd(tempCWD, sizeof(tempCWD));
            if (*command == 'l')
            {
                ls_l(tempCWD);
            }
            else if (*command == 'a')
            {
                ls_a(tempCWD);
            }
            else if (*command == 'A')
            {
                ls_A(tempCWD);
            }
            else if (*command == 'g')
            {
                ls_g(tempCWD);
            }
            else if (*command == 'i')
            {
                ls_i(tempCWD);
            }
        }
        else if (strncmp(command, "cp", 2) == 0)
        {
            cp(command);
        }
        else if (strncmp(command, "mv", 2) == 0)
        {
            mv(command);
        }
        else if (strncmp(command, "grep", 4) == 0)
        {
            grep(command);
        }
        else if (strncmp(command, "ps", 2) == 0)
        {
            ps(command);
        }
        else if (strncmp(command, "man", 3) == 0 || strncmp(command, "help", 4) == 0)
        {
            display_help(command);
        }
        else
        {
            printf("Command not supported\n");
        }
    }
    return 0;
}
void display_help(char *command)
{
    if (strncmp(command, "man", 3) == 0)
    {
        command += 3;
    }
    else
    {
        command += 4;
    }
    *(command - 1) = '\0';
    trim(command);
    if (strncmp(command, "ls", 2) == 0)
    {
        printf("ls - list directory contents\n");
        printf("Flags:\n");
        printf("-a - show hidden files\n");
        printf("-A - show hidden files except for '.' and '..'\n");
        printf("-g - show group information instead of owner\n");
        printf("-i - show inode number of each file\n");
        printf("-l - long listing format\n");
    }
    else if (strncmp(command, "ps", 2) == 0)
    {
        printf("ps - report a snapshot of the current processes\n");
        printf("Flags:\n");
        printf("-e - display all processes\n");
        printf("-a - display all processes with a terminal\n");
        printf("-A - Select all processes.  Identical to -e.\n");
        printf("-d - display all processes except for session leaders\n");
        printf("-f - display full format listing\n");
    }
    else if (strncmp(command, "cp", 2) == 0)
    {
        printf("cp - copy files and directories\n");
        printf("Flags:\n");
        printf("-l - make hard links instead of copying\n");
        printf("-f - force overwrite of existing files\n");
        printf("-p - preserve file attributes while copying\n");
        printf("-v - verbose output\n");
        printf("-i - prompt before overwriting existing files\n");
    }
    else if (strncmp(command, "mv", 2) == 0)
    {
        printf("mv - move or rename files and directories\n");
        printf("Flags:\n");
        printf("-f - force overwrite of existing files\n");
        printf("-v - verbose output\n");
        printf("-i - prompt before overwriting existing files\n");
        printf("-n - do not overwrite existing files\n");
    }
    else if (strncmp(command, "grep", 4) == 0)
    {
        printf("grep - search for patterns in files\n");
        printf("Flags:\n");
        printf("-i - ignore case distinctions\n");
        printf("-c - print only a count of matching lines\n");
        printf("-n - print line numbers with matching lines\n");
        printf("-w - match only whole words\n");
        printf("-y - Obsolete synonym for -i.\n");
    }
    else if (strncmp(command, "cd", 2) == 0)
    {
        printf("cd - change the current working directory\n");
    }
    else if (strncmp(command, "exit", 4) == 0)
    {
        printf("exit - exit the terminal\n");
    }
    else if ((strncmp(command, "help", 4) == 0)|| strlen(command)==0)
    {
    help:
        printf("help/man - display help information about the available commands\n");
    }
    else if (strncmp(command, "man", 3) == 0)
    {
    man:
        printf("help/man - display help information about the available commands\n");
    }
    else
    {
        printf("Unknown command '%s'.\n", command);
    }
}
