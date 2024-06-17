
// include/linux/sched.h
//SSU struct task_struct {

void minisystem();

// create()
#define FILE_TYPE 0
#define DIRECTORY_TYPE 1

int create(const char *path, int type);
void list_directory();
void change_directory();
void create_directory();
void create_file();