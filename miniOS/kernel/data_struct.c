#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_FILES 100
#define MAX_FILENAME 255
#define MAX_FILESIZE 1024

// 파일 메타데이터 구조체
typedef struct
{
    char name[MAX_FILENAME];
    size_t size;
    time_t creation_time;
    time_t modification_time;
    char data[MAX_FILESIZE];
} File;

// 디렉토리 구조체
typedef struct
{
    char name[MAX_FILENAME];
    File *files[MAX_FILES];
    int file_count;
} Directory;

// 루트 디렉토리 생성
Directory root = {.name = "/", .file_count = 0};

// 파일 생성 함수
File *create_file(const char *name)
{
    File *new_file = (File *)malloc(sizeof(File));
    strncpy(new_file->name, name, MAX_FILENAME);
    new_file->size = 0;
    new_file->creation_time = time(NULL);
    new_file->modification_time = time(NULL);
    return new_file;
}

// 파일 추가 함수
void add_file(Directory *dir, File *file)
{
    if (dir->file_count < MAX_FILES)
    {
        dir->files[dir->file_count++] = file;
    }
    else
    {
        printf("Directory is full!\n");
    }
}

// 파일 읽기 함수
void read_file(File *file)
{
    printf("File: %s\n", file->name);
    printf("Size: %lu bytes\n", file->size);
    printf("Created: %s", ctime(&file->creation_time));
    printf("Modified: %s", ctime(&file->modification_time));
    printf("Data: %s\n", file->data);
}

// 파일 쓰기 함수
void write_file(File *file, const char *data)
{
    strncpy(file->data, data, MAX_FILESIZE);
    file->size = strlen(data);
    file->modification_time = time(NULL);
}

int main()
{
    // 파일 생성 및 추가
    File *file1 = create_file("file1.txt");
    write_file(file1, "Hello, MiniOS!");
    add_file(&root, file1);

    // 파일 읽기
    read_file(file1);

    return 0;
}
