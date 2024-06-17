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

void change_directory(char* directory)
{
    if (chdir(directory) == 0) {
    	if (getcwd(current_directory, sizeof(current_directory)) == NULL) {
    		perror("Error getting current directory");
    	}
    } else {
    	perror("Error changing directory");
    }
}
