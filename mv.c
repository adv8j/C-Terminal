#include "headers.h"
#define MAX_FILENAME_LENGTH 100
void trim(char *s);
int mv(char *command)
{
    char source_file[MAX_FILENAME_LENGTH];
    char dest_file[MAX_FILENAME_LENGTH];
    bool force_overwrite = false;
    bool verbose_output = false;
    bool prompt_overwrite = false;
    bool no_clobber = false;
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
        case 'f':
            force_overwrite = true;
            break;
        case 'v':
            verbose_output = true;
            break;
        case 'i':                    // added
            prompt_overwrite = true; // added
            break;
        case 'n':              // added
            no_clobber = true; // added
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
            char choice;
            if (prompt_overwrite)
            {
                printf("cp: overwrite '%s'? (y/n) ", dest_file);
                scanf("%c", &choice);
            }
            else if (no_clobber)
            {
                choice = 'n';
            }
            else
            {
                choice='y';
            }
            
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
    if (verbose_output)
    {
        printf("'%s' -> '%s'\n", source_file, dest_file);
    }
    if (remove(source_file) != 0)
    {
        printf("Error: Failed to remove source file '%s'\n", source_file);
        return 1;
    }
    return 0;
}
