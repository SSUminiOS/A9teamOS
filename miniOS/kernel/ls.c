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

void list_directory() {
    printf("%s의 내용:\n", current_directory);
    DIR *dir;
    struct dirent *entry;
    if ((dir = opendir(current_directory)) != NULL) {
        while ((entry = readdir(dir)) != NULL) {
            printf("%s\n", entry->d_name);
        }
        closedir(dir);
    } else {
        perror("opendir");
    }
}
