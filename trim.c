#include "headers.h"
void trim(char *s)
{
    char s2[strlen(s)];
    strcpy(s2, "");
    for (size_t i = 0; i < strlen(s); i++)
    {
        if (s[i] != ' ')
        {
            strcpy(s2, s + i);
            break;
        }
    }
    for (size_t i = 0; i < strlen(s2); i++)
    {
        if (s2[strlen(s2) - 1 - i] != ' ')
        {
            strncpy(s2, s2, strlen(s2) - i);
            s2[strlen(s2) - i] = '\0';
            break;
        }
    }
    strcpy(s, s2);
}