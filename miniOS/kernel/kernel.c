#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "system.h"

void print_minios(char* str);
int create(const char *path, int type);

int main() {
    print_minios("[MiniOS SSU] Hello, World!");

    char *input, *path;

    while(1) {
        // readline을 사용하여 입력 받기
        input = readline("커맨드를 입력하세요(종료:exit) : ");

        if (strcmp(input,"exit") == 0) {
            break;
        }

        if (strcmp(input,"minisystem") == 0) {
            minisystem();
        }

        else if(strcmp(input,"backup")==0){
                int pid=fork();
                if(pid==0){
                        char* cmd=readline(">> ");
                        char* arr[10]={0,};
                        arr[0]="./kernel/20182672/backup";
                        arr[1]=strtok(cmd," ");

                        for(int i=2;i<10;i++){
                                arr[i]=strtok(NULL," ");
                                if(arr[i]==NULL)
                                        break;
                        }
			
                        if(execvp("./kernel/20182672/backup",arr)<0){
				fprintf(stderr,"exevp backup failed\n");
			}
			printf("not executed child process exiting,,,\n");
			free(input);
			exit(0);
                }
                else{
                        int stat;
                        wait(&stat);
                }
        }

        else system(input);
    }

    // 메모리 해제
    free(input);
    print_minios("[MiniOS SSU] MiniOS Shutdown........");

    return(1);
}

void print_minios(char* str) {
        printf("%s\n",str);
}