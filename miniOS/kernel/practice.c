#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGE_SIZE 4096  // 4KB
#define MEMORY_SIZE 65536  // 64KB
#define NUM_PAGES (MEMORY_SIZE / PAGE_SIZE)
#define DISK_SIZE (NUM_PAGES * PAGE_SIZE)  // 예제용 디스크 크기 (메모리 크기와 동일하게 설정)

typedef struct {
    int frame_number;
    bool is_allocated;
    bool is_in_memory;
} PageTableEntry;

typedef struct {
    PageTableEntry pages[NUM_PAGES];
    int next_free_page;
} MemoryManager;

MemoryManager memory_manager;
char disk[DISK_SIZE];  // 디스크 시뮬레이션

void initialize_memory_manager() {
    for (int i = 0; i < NUM_PAGES; i++) {
        memory_manager.pages[i].frame_number = -1;
        memory_manager.pages[i].is_allocated = false;
        memory_manager.pages[i].is_in_memory = false;
    }
    memory_manager.next_free_page = 0;
}

void* allocate_memory(int size) {
    int num_pages_needed = (size + PAGE_SIZE - 1) / PAGE_SIZE;
    int start_page = -1;
    int contiguous_pages = 0;

    for (int i = 0; i < NUM_PAGES; i++) {
        if (!memory_manager.pages[i].is_allocated) {
            if (start_page == -1) {
                start_page = i;
            }
            contiguous_pages++;
            if (contiguous_pages == num_pages_needed) {
                break;
            }
        } else {
            start_page = -1;
            contiguous_pages = 0;
        }
    }

    if (contiguous_pages < num_pages_needed) {
        return NULL;  // Not enough memory
    }

    for (int i = start_page; i < start_page + num_pages_needed; i++) {
        memory_manager.pages[i].is_allocated = true;
        memory_manager.pages[i].is_in_memory = true;
        memory_manager.pages[i].frame_number = i;  // Frame number is the same as page number in this simple example
    }

    return (void*)(start_page * PAGE_SIZE);
}

void free_memory(void* ptr, int size) {
    int start_page = (int)ptr / PAGE_SIZE;
    int num_pages_to_free = (size + PAGE_SIZE - 1) / PAGE_SIZE;

    for (int i = start_page; i < start_page + num_pages_to_free; i++) {
        memory_manager.pages[i].is_allocated = false;
        memory_manager.pages[i].is_in_memory = false;
        memory_manager.pages[i].frame_number = -1;
    }
}

void handle_page_fault(int page_number) {
    printf("Page fault occurred for page %d\n", page_number);

    // Simple page replacement policy: First free page
    for (int i = 0; i < NUM_PAGES; i++) {
        if (!memory_manager.pages[i].is_in_memory) {
            // Simulate loading the page from disk to memory
            memory_manager.pages[i].frame_number = page_number;
            memory_manager.pages[i].is_in_memory = true;
            printf("Loaded page %d into frame %d\n", page_number, i);
            return;
        }
    }
    // 만약에 free memory가 없다면 replacement 알고리즘이 필요하다.
    // 지금 코드는 replacement 알고리즘이 구현되어 있진 않다.
    printf("No free frame available to load page %d\n", page_number);
}

void access_memory(int page_number) {
    if (!memory_manager.pages[page_number].is_in_memory) {
        handle_page_fault(page_number);
    }
    printf("Accessing page %d in frame %d\n", page_number, memory_manager.pages[page_number].frame_number);
}

void print_memory_status() {
    for (int i = 0; i < NUM_PAGES; i++) {
        printf("Page %d: %s (Frame %d)\n", i, memory_manager.pages[i].is_allocated ? "Allocated" : "Free",
               memory_manager.pages[i].frame_number);
    }
}

// int main() {
//     initialize_memory_manager();
//     print_memory_status();

//     void* ptr1 = allocate_memory(8000);
//     printf("Allocated 8000 bytes at %p\n", ptr1);
//     print_memory_status();

//     void* ptr2 = allocate_memory(16000);
//     printf("Allocated 16000 bytes at %p\n", ptr2);
//     print_memory_status();

//     free_memory(ptr1, 8000);
//     printf("Freed 8000 bytes from %p\n", ptr1);
//     print_memory_status();

//     access_memory(2);  // This should cause a page fault
//     access_memory(3);  // This should cause a page fault

//     return 0;
// }

int main() {
    initialize_memory_manager();
    print_memory_status();

    void* ptr1 = allocate_memory(8000);
    printf("Allocated 8000 bytes at %p\n", ptr1);
    print_memory_status();

    void* ptr2 = allocate_memory(16000);
    printf("Allocated 16000 bytes at %p\n", ptr2);
    print_memory_status();

    free_memory(ptr1, 8000);
    printf("Freed 8000 bytes from %p\n", ptr1);
    print_memory_status();

    // Accessing pages that will cause page faults
    access_memory(2);  // This should cause a page fault and load the page into memory
    access_memory(3);  // This should cause another page fault and load the page into memory

    // Simulating more page faults
    access_memory(10);  // This should cause a page fault
    access_memory(20);  // This should cause a page fault

    return 0;
}