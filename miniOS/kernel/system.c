#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "system.h"

void minisystem()
{
    printf("minisystem\n");
}

// create()
#define FILE_TYPE 0
#define DIRECTORY_TYPE 1

int create(const char *path, int type);

// 파일 또는 디렉토리를 생성하는 함수
int create(const char *path, int type) {
    FILE *fp;

    if (type == FILE_TYPE) {
        // 파일 생성
        fp = fopen(path, "w");
        if (fp == NULL) {
            perror("Failed to create file");
            return -1;
        }
        fclose(fp);
    }
    else if (type == DIRECTORY_TYPE) {
        // 디렉토리 생성
        if (create(path, 0755) == -1) {  // 0755는 일반적으로 사용되는 권한 설정입니다.
            perror("Failed to create directory");
            return -1;
        }
    }

    return 0;  // 성공 시 0 반환
}