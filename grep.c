#include "headers.h"
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

#define MAX_LINE_LENGTH 1000
#define MAX_FILENAME_LENGTH 100
#define MAX_WORD_LENGTH 100

void trim(char *s);

char *strcasestr(const char *haystack, const char *needle)
{
    size_t needle_len = strlen(needle);

    while (*haystack)
    {
        if (strncasecmp(haystack, needle, needle_len) == 0)
        {
            return (char *)haystack;
        }

        ++haystack;
    }

    return NULL;
}

int grep(char *command)
{
    char word[MAX_WORD_LENGTH];
    char filename[MAX_FILENAME_LENGTH];
    bool ignore_case = false;
    bool word_regexp = false;
    bool line_count = false;
    bool line_number = false;

    command += 4;
    if (command[0] != ' ')
    {
        printf("Error: Unsupported Command\n");
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
        case 'w':
            word_regexp = true;
            break;
        case 'n':
            line_number = true;
            ;
            break;
        case 'i':
        case 'y': // added
            ignore_case = true;
            break;
        case 'c': // added
            line_count = true;
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
            command--;
            command[0] = '-';
        }
    }

    int num_scanned = sscanf(command, "\"%99[^\"]\" %99s", word, filename);

    if (num_scanned != 2)
    {
        num_scanned = sscanf(command, "%99s %99s", word, filename);
        if (num_scanned != 2)
        {
            printf("Error: Invalid command format\n");
            return 1;
        }
    }

    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: Unable to open file '%s'\n", filename);
        return 1;
    }
    char line[MAX_LINE_LENGTH];
    int line_num = 0;
    int match_count = 0;

    while (fgets(line, MAX_LINE_LENGTH, fp) != NULL)
    {
        line_num++;
        if (word_regexp)
        {
            // implement word boundary matching here
            char *p;
            char *lastmatch;
            lastmatch = line;
            if (ignore_case)
            {
                p = strcasestr(line, word);
            }
            else
            {
                p = strstr(line, word);
            }
            while (p != NULL)
            {
                int start_pos = p - line;
                int end_pos = start_pos + strlen(word);
                // Check if the match is at the beginning of the line or has a non-alphanumeric character before it
                if (start_pos == 0 || !isalnum(line[start_pos - 1]))
                {
                    // Check if the match is at the end of the line
                    if (end_pos == strlen(line) || !isalnum(line[end_pos]))
                    {

                        // found a match
                        match_count++;
                        if (line_count)
                        {
                            break; // don't print the line, just count the matches
                        }
                        if (line_number && (lastmatch == line))
                        {
                            printf("%d:", line_num);
                        }
                        char *highlight_start = p;
                        char *highlight_end = p + strlen(word);
                        int length_before_match = highlight_start - lastmatch;
                        printf("%.*s", length_before_match, lastmatch);                      // print everything before the match
                        printf("\033[1;31m%.*s\033[0m", (int)strlen(word), highlight_start); // print the match in bold red
                        // printf("%s", highlight_end);               // print everything after the match
                        lastmatch = highlight_end;
                    }
                }
                if (ignore_case)
                {
                    p = strcasestr(p + strlen(word), word); // search for the next match, case-insensitively
                }
                else
                {
                    p = strstr(p + strlen(word), word); // search for the next match
                }
            }
            if (lastmatch != line)
            {
                printf("%s", lastmatch);
            }
        }
        else
        {
            char *p;
            char *lastmatch;
            lastmatch = line;
            if (ignore_case)
            {
                p = strcasestr(line, word);
            }
            else
            {
                p = strstr(line, word);
            }

            while (p != NULL)
            {
                // found a match
                match_count++;
                if (line_count)
                {
                    break; // don't print the line, just count the matches
                }
                if (line_number && (lastmatch == line))
                {
                    printf("%d:", line_num);
                }
                char *highlight_start = p;
                char *highlight_end = p + strlen(word);
                int length_before_match = highlight_start - lastmatch;
                printf("%.*s", length_before_match, lastmatch);                      // print everything before the match
                printf("\033[1;31m%.*s\033[0m", (int)strlen(word), highlight_start); // print the match in bold red
                // printf("%s", highlight_end);               // print everything after the match
                lastmatch = highlight_end;
                if (ignore_case)
                {
                    p = strcasestr(p + strlen(word), word); // search for the next match, case-insensitively
                }
                else
                {
                    p = strstr(p + strlen(word), word); // search for the next match
                }
            }
            if (lastmatch != line)
            {
                printf("%s", lastmatch);
            }
        }
    }
    if (line_count)
    {
        printf("%d\n", match_count);
    }
    fclose(fp);
    return 0;
}