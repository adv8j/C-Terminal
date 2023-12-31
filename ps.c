#include "headers.h"
#define FORMAT "%5d\t\t%10s\t\t%9s\t\t%s\n"
#define FULL_FORMAT "%-15s\t%5d\t%5d\t%3d %5s %-5s\t%9s\t%s\n"
void trim(char *s);

int ps(char *command)
{
    struct data
    {
        int pid;
        int ppid;
        int sid;
        char cmd[1000];
        unsigned long long int rss;
        long int sz;
        long long int uid;
        char username[256];
        int psr;
        char tty_name[256];
        long unsigned int utime;
        long unsigned int stime;
        time_t STIME;

        char formed_utime[256];
        char formed_STIME[256];
        int c;
    };
    struct data prop;
    DIR *dir;
    struct dirent *ent;
    int i, fd_self, fd;
    char flag, *tty;
    char cmd[512], tty_self[512], path[512], time_s[512];
    FILE *file;
    bool all_proc = false;
    bool all_proc_except_SLeader = false;
    bool all_proc_except_SLeader_diffshell = false;
    bool full_format = false;
    bool only_PSorPSF = true;
    // Check if any argument was provided
    command += 2;
    *(command - 1) = '\0';
    if (!((command[0] == ' ') || (strlen(command) == 0)))
    {
        printf("Error: Unsupported Command \n");
        return 1;
    }
    *(command - 1) = '\0';
    trim(command);
    // flag parsing
    while (command[0] == '-')
    {
        command++;
        switch (command[0])
        {
        case 'e':
        case 'A':
            all_proc = true;
        case 'd': // added
            all_proc_except_SLeader = true;
        case 'a':
            all_proc_except_SLeader_diffshell = true;
            only_PSorPSF = false;
            break;
        case 'f': // added
            full_format = true;
            break;
        default:
            printf("Error: Unsupported flag -%c\n", command[0]);
            return 1;
        }
        command++;
        if (command[0] == ' ')
        {
            *(command - 1) = '\0';
            trim(command);
        }
        else
        {
            if (strlen(command) == 0)
            {
                break;
            }
            command--;
            command[0] = '-';
        }
    }
    dir = opendir("/proc");
    fd_self = open("/proc/self/fd/0", O_RDONLY);
    snprintf(tty_self, 512, "%s", ttyname(fd_self) + 5);
    if (!full_format)
        printf("%5s\t\t%10s\t\t%9s\t\t%s\n", "PID", "TTY", "TIME", "CMD");
    else
        printf("%-15s\t%5s\t%5s\t  %s %5s %-5s\t%9s\t%s\n", "UID", "PID", "PPID", "C", "STIME", "TTY", "TIME", "CMD");
    while ((ent = readdir(dir)) != NULL)
    {
        memset(prop.formed_STIME, '\0', 256);
        memset(prop.formed_utime, '\0', 256);
        // a++;
        // printf("%d\n", a);
        flag = 1;
        for (i = 0; i < strlen(ent->d_name); i++)
        {
            if (!isdigit(ent->d_name[i]))
            {
                flag = 0;
                break;
            }
        }
        if (flag)
        {
            snprintf(path, 512, "/proc/%s/fd/0", ent->d_name);
            fd = open(path, O_RDONLY);
            if (fd != -1)
            {
                tty = ttyname(fd);
                snprintf(path, 512, "/proc/%s/stat", ent->d_name);
                file = fopen(path, "r");
                // fscanf(file, "%d%s%c%c%c", &i, cmd, &flag, &flag, &flag);
                fscanf(file, "%d (%[^)]) %*c %d %*d %d %*d %*d %*u %*d %*d %*d %*d %lu %lu %*d %*d %*d %*d %*d %*d %ld %*d %*u %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %*d %d %d",
                       &prop.pid, prop.cmd, &prop.ppid, &prop.sid, &prop.utime, &prop.stime, &prop.STIME, &prop.psr, &prop.c);
                prop.c = prop.stime / sysconf(_SC_CLK_TCK);
                fclose(file);
                snprintf(path, 512, "/proc/%s/statm", ent->d_name);
                file = fopen(path, "r");
                fscanf(file, "%ld %llu", &prop.sz, &prop.rss);
                prop.rss *= 4;
                fclose(file);
                snprintf(path, 512, "/proc/%s/status", ent->d_name);
                file = fopen(path, "r");
                char line[256];
                memset(line, '\0', 256);
                while (fgets(line, sizeof(line), file))
                {
                    if (strncmp(line, "Uid:", 4) == 0)
                    {
                        sscanf(line, "Uid:\t%lld", &prop.uid);

                        break;
                    }
                }
                fclose(file);
                struct passwd *pwd = malloc(sizeof(struct passwd));
                pwd = getpwuid(prop.uid);
                if (pwd == NULL)
                {
                    continue;
                }
                // strcpy(prop.username, pwd->pw_name);
                snprintf(prop.username, 256, "%s", pwd->pw_name);
                if (ttyname(fd) != NULL)
                    snprintf(prop.tty_name, 256, "%s", ttyname(fd) + 5); // strcpy(prop.tty_name, ttyname(fd));
                else
                    strcpy(prop.tty_name, "?");
                close(fd);
                time_t uptime;
                file = fopen("/proc/uptime", "r");
                fscanf(file, "%ld", &uptime);
                fclose(file);
                prop.STIME = (uptime - (prop.STIME) / sysconf(_SC_CLK_TCK));
                long unsigned int PROG = (prop.utime + prop.stime) / (double)sysconf(_SC_CLK_TCK);
                sprintf(prop.formed_utime, "%02lu:%02lu:%02lu", (PROG / 3600), (PROG / 60) % 60, PROG % 60);

                time_t curr = time(0);
                prop.STIME = curr - prop.STIME;
                struct tm *loop = localtime(&prop.STIME);
                strftime(prop.formed_STIME, 256, "%H:%M", loop);
                if (only_PSorPSF)
                {
                    if (!(strcmp(tty_self, prop.tty_name) == 0))
                        continue;
                }
                else
                {
                    if (all_proc == false)
                    {
                        if (prop.pid == prop.sid)
                            continue;
                        if (all_proc_except_SLeader == false)
                        {
                            if (!(strcmp(tty_self, prop.tty_name) == 0))
                                continue;
                        }
                    }
                }
                if (full_format)
                {
                    printf(FULL_FORMAT, pwd->pw_name, prop.pid, prop.ppid, prop.c, prop.formed_STIME, prop.tty_name, prop.formed_utime, prop.cmd);
                }
                else
                {
                    printf(FORMAT, prop.pid, prop.tty_name, prop.formed_utime, prop.cmd);
                }
            }
        }
    }
    close(fd_self);
    closedir(dir);
    return 0;
}