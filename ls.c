#include "headers.h"
#define MAX_PATH_LENGTH 1024
int compare(const struct dirent **a, const struct dirent **b);
int ls(char *cwd) {
    struct dirent **namelist;
    int n;
    n = scandir(cwd, &namelist, NULL, compare);
    if (n == -1) {
        perror("scandir");
        return 1;
    }
    char path[MAX_PATH_LENGTH];
    for (int i = 0; i < n; i++) {
        sprintf(path, "%s/%s", cwd, namelist[i]->d_name);
        struct stat sb;
        if (lstat(path, &sb) == -1) {
            perror("lstat");
            continue;
        }
        if (namelist[i]->d_name[0] == '.') {
            free(namelist[i]);
            continue;
        }
        if (S_ISDIR(sb.st_mode)) {
            printf("\033[1;34m"); // set color to blue
        } else if (sb.st_mode & S_IXUSR) {
            printf("\033[1;32m"); // set color to green for executables
        } else {
            printf("\033[0m"); // reset color
        }
        printf("\033[1m"); // set bold
        printf("%s\n", namelist[i]->d_name);
        printf("\033[0m"); // reset bold and color
        free(namelist[i]);
    }
    free(namelist);
    return 0;
}
int compare(const struct dirent **a, const struct dirent **b)
{
    return strcasecmp((*a)->d_name, (*b)->d_name);
}