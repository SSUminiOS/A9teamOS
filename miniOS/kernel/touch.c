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

void create_file(char* file_name)
{
    char file_path[MAX_PATH_LENGTH];
    snprintf(file_path, sizeof(file_path), "%s%s", current_directory, file_name);
    
    int fd = open(file_path, O_CREAT | O_EXCL | O_WRONLY, 0644);
    if (fd == -1) {
    	perror("Error creating file");
    } else {
    	printf("File '%s' created successfully.\n", file_name);
    	close(fd);
    }
}
