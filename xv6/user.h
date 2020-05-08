#include "types.h"

struct stat;

char **environ;
int errno;

// system calls
int fork(void);
int _exit(void) __attribute__((noreturn));
int wait(void);
int pipe(int*);
int write(int, void*, int);
int read(int, void*, int);
int close(int);
int kill(int);
int execve(const char*, char* const*, char* const*);
int open(char*, int, ...);
int mknod(char*, short, short, int);
int unlink(char*);
int fstat(int fd, struct stat*);
int link(char*, char*);
int mkfifo(char*, int);
int mkdir(char*, int);
int chdir(char*);
int dup(int);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int umask(int);
int setreuid(int, int);
int setregid(int, int);
int getuid(void);
int geteuid(void);
int getgid(void);
int getegid(void);
int chmod(const char*, mode_t);
int chown(const char*, uid_t, gid_t);
int setgroups(uint, gid_t*);
int getgroups(int, gid_t*);
int exit_group(void) __attribute__((noreturn));
int sched_yield(void);
int mount(char*, char*);
int chroot(char*);
char* mmap(char*, int, int, int, int, int);

// ulib.c
int stat(char*, struct stat*);
char* strcpy(char*, char*);
void *memmove(void*, void*, int);
char* strchr(const char*, char c);
int strcmp(const char*, const char*);
char* safestrcpy(char *s, const char *t, int n);
void printf(int, char*, ...);
char* gets(char*, int max);
char* fgets(char*, int, int);
uint strlen(const char*);
void* memset(void*, int, uint);
void* malloc(uint);
void free(void*);
int atoi(const char*);
int strncmp(const char*, const char*, uint num);
char* strncpy(char *s, const char *t, int n);
char* strrchr(const char *s, int c);
char* strchrnul(const char *s, int c);
char* getenv(const char *name);
int execvpe(const char *file, char *const argv[], char *const envp[]);
int clone_fn(int (*start_routine)(void*), void* stack, void *arg);
int exit(void) __attribute__((noreturn));

// thread.c
int thread_create(thread_t* thread, void* (*fn)(void*), void* arg, int);
int thread_join(thread_t thread, void** retval);
int thread_detach(thread_t thread);
