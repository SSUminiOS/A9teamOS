#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// 메모리 할당 로그 구조체
typedef struct MemoryLog {
    void *address;
    size_t size;
    char file[256];
    int line;
} MemoryLog;

// 로그 배열과 로그 카운트
#define MAX_LOGS 1024
MemoryLog logs[MAX_LOGS];
int log_count = 0;

// 메모리 할당 함수
void *allocate_memory(size_t size, const char *file, int line) {
    if (log_count >= MAX_LOGS) {
        fprintf(stderr, "Out of memory log slots!\n");
        return NULL;
    }

    void *ptr = malloc(size);
    if (!ptr) {
        perror("Failed to allocate memory");
        return NULL;
    }

    // 로그에 추가
    logs[log_count].address = ptr;
    logs[log_count].size = size;
    strncpy(logs[log_count].file, file, sizeof(logs[log_count].file) - 1);
    logs[log_count].line = line;
    log_count++;

    return ptr;
}

// 메모리 해제 함수
void free_memory(void *ptr, const char *file, int line) {
    for (int i = 0; i < log_count; i++) {
        if (logs[i].address == ptr) {
            free(ptr);
            // 로그에서 제거
            logs[i] = logs[--log_count];
            return;
        }
    }

    fprintf(stderr, "Attempt to free untracked memory at %s:%d\n", file, line);
}

// 메모리 로그 출력 함수
void print_memory_logs() {
    printf("Active memory allocations:\n");
    for (int i = 0; i < log_count; i++) {
        printf("Allocated at %s:%d, size: %zu bytes\n", logs[i].file, logs[i].line, logs[i].size);
    }
}

// 매크로를 사용하여 파일명과 라인 번호 자동 삽입
#define ALLOCATE(size) allocate_memory(size, __FILE__, __LINE__)
#define FREE(ptr) free_memory(ptr, __FILE__, __LINE__)

// 메모리 관리 모듈 사용 예제
int main() {
    int *array = ALLOCATE(sizeof(int) * 100);
    if (!array) return -1;

    // 작업 수행
    array[0] = 10;

    FREE(array);
    print_memory_logs(); // 로그는 비어있어야 함

    return 0;
}
