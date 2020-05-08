#include "pwd.h"
#include "fcntl.h"
#include "user.h"

static struct passwd current_passwd;
static char current_line[MAX_PASSWD_LINE_LENGTH];
static int fd;

// This function assumes that fd is already open
// Returns -1 if could not read next entry
static int
get_and_parse_pwent(void)
{
  char* tokens[NUMBER_OF_PASSWD_TOKENS];
  int ok = 0;
  fgets(current_line, MAX_PASSWD_LINE_LENGTH, fd);
  int length = strlen(current_line);
  if (current_line[length - 1] == '\n' ||
      current_line[length - 1] == '\r') {
    current_line[length - 1] = 0;
  }
  int next_token = 0;
  int i;
  for (i = 0; current_line[i]; ++i) {
    if (current_line[i] == ':') {
      if (ok == 0) {
        tokens[next_token++] = current_line + i;
      }
      current_line[i] = 0;
      ok = 0;
    } else if (ok == 0 && next_token < NUMBER_OF_PASSWD_TOKENS) {
      ok = 1;
      tokens[next_token++] = current_line + i;
    }
  }
  if (i == 0) return -1;
  current_passwd.pw_name = tokens[0];
  current_passwd.pw_passwd = tokens[1];
  current_passwd.pw_uid = atoi(tokens[2]);
  current_passwd.pw_gid = atoi(tokens[3]);
  current_passwd.pw_gecos = tokens[4];
  current_passwd.pw_dir = tokens[5];
  current_passwd.pw_shell = tokens[6];
  return 0;
}

struct passwd*
getpwent(void)
{
  if (fd == 0) {
    fd = open(PASSWD_FILE, O_RDONLY);
    if (fd < 0) {
      fd = 0;
      return 0;
    }
  }
  if (get_and_parse_pwent() == -1) {
    return 0;
  }
  return &current_passwd;
}

void
setpwent(void)
{
  if (fd != 0) {
    close(fd);
    fd = 0;
  }
  fd = open(PASSWD_FILE, O_RDONLY);
  if (fd < 0) fd = 0;
}

void
endpwent(void)
{
  if (fd != 0) {
    close(fd);
    fd = 0;
  }
}

struct passwd*
getpwnam(const char* name)
{
  setpwent();
  while (getpwent()) {
    if (strcmp(name, current_passwd.pw_name) == 0) {
      endpwent();
      return &current_passwd;
    }
  }
  endpwent();
  return 0;
}

struct passwd*
getpwuid(uid_t uid)
{
  setpwent();
  while (getpwent()) {
    if (current_passwd.pw_uid == uid) {
      endpwent();
      return &current_passwd;
    }
  }
  endpwent();
  return 0;
}

int
putpwent(struct passwd* pass, int fd)
{
  if (pass == 0) return -1;
  printf(fd, "%s:%s:%d:%d:%s:%s:%s\n",
      pass->pw_name,
      pass->pw_passwd,
      pass->pw_uid,
      pass->pw_gid,
      pass->pw_gecos,
      pass->pw_dir,
      pass->pw_shell);
  return 0;
}