#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_COMMAND_LENGTH 100
#define MAX_PATH_LENGTH 1000

char current_directory[MAX_PATH_LENGTH];
void print_minios(char* str);

void create_directory(char* directory_name)
{
    char new_directory[MAX_PATH_LENGTH];
    sprintf(new_directory, "%s/%s", current_directory, directory_name);
    int status = mkdir(new_directory, 0777);
    if (status == 0)
    {
        printf("Directory '%s' created successfully\n", new_directory);
    }
    else
    {
        printf("Failed to create directory '%s'\n", new_directory);
    }
}
