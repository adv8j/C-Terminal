#include "headers.h"
#define MAX_FILENAME_LENGTH 100
void trim(char *s);
int cp(char *command)
{
    char source_file[MAX_FILENAME_LENGTH];
    char dest_file[MAX_FILENAME_LENGTH];
    bool create_symbolic_link = false;
    bool force_overwrite = false;
    bool preserve_permissions = false;
    bool verbose_output = false;
    bool prompt_overwrite = false;
    command += 2;
    if (command[0] != ' ')
    {
        printf("Error: Unsupported Command`");
        return 1;
    }
    *(command - 1) = '\0';
    trim(command);
    // Parse the flags
    if (command[0] == '-')
    {
        command++;
        switch (command[0])
        {
        case 'l':
            create_symbolic_link = true;
            break;
        case 'f':
            force_overwrite = true;
            break;
        case 'p':
            preserve_permissions = true;
            break;
        case 'v':
            verbose_output = true;
            break;
        case 'i':                    // added
            prompt_overwrite = true; // added
            break;
        default:
            printf("Error: Unsupported flag -%c\n", command[0]);
            return 1;
        }
        command++;
        if (command[0] != ' ')
        {
            printf("Error: Unsupported Command`");
            return 1;
        }
        command++;
    }
    *(command - 1) = '\0';
    trim(command);
    // Parse the command to extract the source file and destination file names
    int num_scanned = sscanf(command, "%99s %99s", source_file, dest_file);
    if (num_scanned != 2)
    {
        printf("Error: Invalid command\n");
        return 1;
    }
    // Call the code that copies the files
    FILE *src_file = fopen(source_file, "rb");
    if (!src_file)
    {
        printf("Error: Failed to open source file '%s': %s\n", source_file, strerror(errno));
        return 1;
    }

    FILE *dest_file_ptr;
    if (create_symbolic_link)
    {
        dest_file_ptr = fopen(dest_file, "w");
    }
    else
    {
        if (force_overwrite)
        {
            // open the destination file in write mode and overwrite it
            dest_file_ptr = fopen(dest_file, "wb");
        }
        else
        {
            // open the destination file in read mode and check if it already exists
            dest_file_ptr = fopen(dest_file, "rb");
            if (dest_file_ptr)
            {
                fclose(dest_file_ptr);
                printf("cp: overwrite '%s'? (y/n) ", dest_file);
                char choice;
                scanf("%c", &choice);
                if (choice != 'y' && choice != 'Y')
                {
                    printf("not overwritten\n");
                    fclose(src_file);
                    return 0;
                }
            }
            // open the destination file in write mode and copy its contents
            dest_file_ptr = fopen(dest_file, "wb");
        }
    }

    if (!dest_file_ptr)
    {
        printf("Error: Failed to open destination file '%s': %s\n", dest_file, strerror(errno));
        fclose(src_file);
        return 1;
    }

    char buffer[1024];
    size_t n;

    while ((n = fread(buffer, 1, sizeof(buffer), src_file)) > 0)
    {
        if (fwrite(buffer, 1, n, dest_file_ptr) != n)
        {
            printf("Error: Failed to write to destination file '%s': %s\n", dest_file, strerror(errno));
            fclose(src_file);
            fclose(dest_file_ptr);
            return 1;
        }
    }

    fclose(src_file);
    fclose(dest_file_ptr);

    if (preserve_permissions)
    {
        struct stat src_stat;
        struct stat dest_stat;
        if (stat(source_file, &src_stat) != 0)
        {
            printf("Error: Failed to get permissions for '%s': %s\n", source_file, strerror(errno));
            return 1;
        }
        if (stat(dest_file, &dest_stat) != 0)
        {
            printf("Error: Failed to get permissions for '%s': %s\n", dest_file, strerror(errno));
            return 1;
        }
        if (chmod(dest_file, src_stat.st_mode) != 0)
        {
            printf("Error: Failed to preserve permissions for '%s': %s\n", dest_file, strerror(errno));
            return 1;
        }
    }

    if (verbose_output)
    {
        printf("'%s' -> '%s'\n", source_file, dest_file);
    }

    return 0;
}

