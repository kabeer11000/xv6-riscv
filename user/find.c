// Copyright (c) Kabeers Network Authors

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char *get_filename(char *path)
{
  char *p = path + strlen(path);
  while (p >= path && *p != '/')
    p--;
  return p + 1;
}

void find_file(char *path, char *target)
{
  int fd;
  struct stat st;
  struct dirent de;
  char new_path[512];

  fd = open(path, 0);
  if (fd < 0)
  {
    printf("cannot open %s\n", path);
    return;
  }

  if (fstat(fd, &st) < 0)
  {
    printf("cannot stat %s\n", path);
    close(fd);
    return;
  }

  if (st.type == T_FILE)
  {
    if (strcmp(get_filename(path), target) == 0)
      printf("%s\n", path);
  }
  else if (st.type == T_DIR)
  {
    while (read(fd, &de, sizeof(de)) == sizeof(de))
    {
      if (de.inum == 0)
        continue;
      if (strcmp(de.name, ".") == 0 || strcmp(de.name, "..") == 0)
        continue;

      strcpy(new_path, path);
      if (path[strlen(path) - 1] != '/')
        strcat(new_path, "/");

      int len = strlen(new_path);
      memmove(new_path + len, de.name, DIRSIZ);
      new_path[len + DIRSIZ] = 0;

      find_file(new_path, target);
    }
  }

  close(fd);
}

int main(int argc, char *argv[])
{
  if (argc != 3)
  {
    printf("Usage: find path name\n");
    exit(1);
  }

  find_file(argv[1], argv[2]);
  exit(0);
}