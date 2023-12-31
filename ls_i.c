#include "headers.h"
int ls_i_actual_implementation(char *filename);
int compare(const struct dirent **a, const struct dirent **b);
int ls_i(char *cwd)
{
    struct dirent **namelist;
    int n;
    n = scandir(cwd, &namelist, NULL, compare);
    if (n == -1)
    {
        perror("scandir");
        return 1;
    }
    for (int i = 0; i < n; i++)
    {
        if(*(namelist[i]->d_name)=='.')
        continue;
        ls_i_actual_implementation(namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);
    return 0;
}
int ls_i_actual_implementation(char *filename)
{
    struct stat my_stat;
    if (stat(filename, &my_stat) < 0) // 0 if okay, -1 on error
    {
        printf("\nError\n");
        exit(0);
    }
    printf("%8ld ", my_stat.st_ino);
    if (S_ISDIR(my_stat.st_mode))
        printf("\e[1m\033[34m%s\033[0m\e[m\n", filename); // blue for directories
    else if (my_stat.st_mode & S_IXUSR || my_stat.st_mode & S_IXGRP || my_stat.st_mode & S_IXOTH)
        printf("\e[1m\033[32m%s\033[0m\e[m\n", filename); // green for executables
    else
        printf("\e[1m%s\e[m\n", filename); // no color for regular files
    return 0;
}