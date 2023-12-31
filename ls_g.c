#include "headers.h"
int ls_g_actual_implementation(char *filename);
int compare(const struct dirent **a, const struct dirent **b);
int ls_g(char *cwd)
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
        ls_g_actual_implementation(namelist[i]->d_name);
        free(namelist[i]);
    }
    free(namelist);
    return 0;
}
int ls_g_actual_implementation(char *filename)
{
    struct stat my_stat;
    if (stat(filename, &my_stat) < 0) // 0 if okay, -1 on error
    {
        printf("\nError\n");
        exit(0);
    }
    printf((S_ISDIR(my_stat.st_mode)) ? "d" : "-");
    printf((my_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((my_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((my_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((my_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((my_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((my_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((my_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((my_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((my_stat.st_mode & S_IXOTH) ? "x " : "- ");
    printf("%2ld ", my_stat.st_nlink);
    struct group *group_name = getgrgid(my_stat.st_gid);
    printf("%s ", group_name->gr_name);
    printf("%12ld ", my_stat.st_size);
    char datestring[256];
    strftime(datestring, sizeof(datestring), "%b %d %H:%M", localtime(&my_stat.st_mtime));
    printf("%s ", datestring);

    if (S_ISDIR(my_stat.st_mode))
        printf("\e[1m\033[34m%s\033[0m\e[m\n", filename); // blue for directories
    else if (my_stat.st_mode & S_IXUSR || my_stat.st_mode & S_IXGRP || my_stat.st_mode & S_IXOTH)
        printf("\e[1m\033[32m%s\033[0m\e[m\n", filename); // green for executables
    else
        printf("\e[1m%s\e[m\n", filename); // no color for regular files
    return 0;
}