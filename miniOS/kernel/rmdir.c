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

void remove_directory(char *directory){
    char directory_path[MAX_PATH_LENGTH];
    snprintf(directory_path, sizeof(directory_path), "%s%s", current_directory, directory);
    
    if(rmdir(directory_path) == 0){
        printf("Directory '%s' removed successfully.\n", directory);
    } else {
        perror("Error removing directory");
    }
}
