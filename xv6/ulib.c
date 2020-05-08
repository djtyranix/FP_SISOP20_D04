#include "types.h"
#include "stat.h"
#include "fcntl.h"
#include "user.h"
#include "x86.h"
#include "errno.h"
#include "syscall.h"
#include "clone_flags.h"

char **environ = 0;

char*
strcpy(char *s, char *t)
{
  char *os;

  os = s;
  while((*s++ = *t++) != 0)
    ;
  return os;
}

int
strcmp(const char *p, const char *q)
{
  while(*p && *p == *q)
    p++, q++;
  return (uchar)*p - (uchar)*q;
}

uint
strlen(const char *s)
{
  int n;

  for(n = 0; s[n]; n++)
    ;
  return n;
}

char*
safestrcpy(char *s, const char *t, int n)
{
  char *os;
  
  os = s;
  if(n <= 0)
    return os;
  while(--n > 0 && (*s++ = *t++) != 0)
    ;
  *s = 0;
  return os;
}

void*
memset(void *dst, int c, uint n)
{
  stosb(dst, c, n);
  return dst;
}

char*
strchr(const char *s, char c)
{
  for(; *s; s++)
    if(*s == c)
      return (char*)s;
  return 0;
}

char*
gets(char *buf, int max)
{
  return fgets(buf, max, 0);
}

char*
fgets(char* buf, int size, int fd)
{
  int i;
  char c;

  for(i = 0; i + 1 < size;){
    int cc = read(fd, &c, 1);
    if(cc < 1)
      break;
    buf[i++] = c;
    if(c == '\n' || c == '\r')
      break;
  }
  buf[i] = '\0';
  return buf;
}

int
stat(char *n, struct stat *st)
{
  int fd;
  int r;

  fd = open(n, O_RDONLY | O_NONBLOCK);
  if(fd < 0)
    return -1;
  r = fstat(fd, st);
  close(fd);
  return r;
}

int
atoi(const char *s)
{
  int n;

  n = 0;
  while('0' <= *s && *s <= '9')
    n = n*10 + *s++ - '0';
  return n;
}

void*
memmove(void *vdst, void *vsrc, int n)
{
  char *dst, *src;
  
  dst = vdst;
  src = vsrc;
  while(n-- > 0)
    *dst++ = *src++;
  return vdst;
}

int
strncmp(const char* p, const char* q, uint n)
{
  while(n > 0 && *p && (*p == *q))
    n--, p++, q++;
  if(n == 0)
    return 0;
  return (int)*p - (int)*q;
}

char*
strncpy(char *s, const char *t, int n)
{
  char *os;
  
  os = s;
  while(n-- > 0 && (*s++ = *t++) != 0)
    ;
  while(n-- > 0)
    *s++ = 0;
  return os;
}

char*
strrchr(const char *s, int c)
{
  char *p = (char*)s;
  char *result = 0;
  while (*p) {
    if (*p == c) {
      result = p;
    }
    p++;
  }
  if (*p == c) return p;
  return result;
}

char*
strchrnul(const char* s, int c)
{
  char *p = (char*)s;
  while (*p && *p != c) {
    p++;
  }
  return p;
}

char*
getenv(const char *name)
{
  int length = strlen(name);
  for (int i = 0; environ[i]; ++i) {
    if (strncmp(environ[i], name, length) == 0 &&
        environ[i][length] == '=') {
      return environ[i] + length + 1;
    }
  }
  return 0;
}

#define STACKSIZE 4096

int
clone_fn(int (*fn)(void*), void* stack, void* arg)
{
  int retval;
  if (stack == 0)
  {
    stack = (void*)malloc(STACKSIZE);
    stack = (void*)(stack + STACKSIZE);
  }
  stack = (void*)(((void**)stack) - 1);
  *(void**)stack = arg;
  __asm__ __volatile__(
      "pushl %5\n\t"
      "pushl %4\n\t"
      "pushl %1\n\t"
      "int $0x40\n\t"   /* xv6 system call */
      "testl %0,%0\n\t" /* check return value */
      "jne f1\n\t"    /* jump if parent */
      "call *%3\n\t"    /* start subthread function */
      "movl %2,%0\n\t"
      "int $0x40\n"   /* exit system call: exit subthread */
      "f1:\t"
      :"=a" (retval)
      :"0" (SYS_clone),"i" (SYS__exit),
      "r" (fn),
      "b" (stack),
      "c" (CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_THREAD));
  if (retval < 0)
  {
    errno = -retval;
    retval = -1;
  }
  return retval;
}

int
exit(void)
{
  exit_group();
}
