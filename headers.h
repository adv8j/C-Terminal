#ifndef HEADERS_H
#define HEADERS_H

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
#define MAX 1000
#endif /* HEADERS_H */
