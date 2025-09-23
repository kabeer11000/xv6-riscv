#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"
#include "kernel/fcntl.h"

char*
basename(char *path)
{
  char *p;

  for(p = path + strlen(path); p >= path && *p != '/'; p--)
    ;
  p++;
  return p;
}

void
find(char *path, char *name, char *exec_cmd)
{
  char buf[512], *p;
  int fd;
  struct dirent de;
  struct stat st;
  int pid;

  if((fd = open(path, O_RDONLY)) < 0){
    fprintf(2, "find: cannot open %s\n", path);
    return;
  }

  if(fstat(fd, &st) < 0){
    fprintf(2, "find: cannot stat %s\n", path);
    close(fd);
    return;
  }

  switch(st.type){
  case T_FILE:
    if(strcmp(basename(path), name) == 0){
      printf("%s\n", path);
      if(exec_cmd){
        if((pid = fork()) == 0){
          char *args[] = {exec_cmd, path, 0};
          exec(exec_cmd, args);
          fprintf(2, "find: exec %s failed\n", exec_cmd);
          exit(1);
        } else if(pid > 0){
          wait(0);
        }
      }
    }
    break;

  case T_DIR:
    if(strlen(path) + 1 + DIRSIZ + 1 > sizeof buf){
      fprintf(2, "find: path too long\n");
      break;
    }
    strcpy(buf, path);
    p = buf + strlen(buf);
    *p++ = '/';
    while(read(fd, &de, sizeof(de)) == sizeof(de)){
      if(de.inum == 0)
        continue;
      memmove(p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;
      if(strcmp(p, ".") == 0 || strcmp(p, "..") == 0)
        continue;
      find(buf, name, exec_cmd);
    }
    break;
  }
  close(fd);
}

int
main(int argc, char *argv[])
{
  char *exec_cmd = 0;

  if(argc < 3){
    fprintf(2, "Usage: find path name [-exec command]\n");
    exit(1);
  }

  if(argc >= 5 && strcmp(argv[3], "-exec") == 0){
    exec_cmd = argv[4];
  }

  find(argv[1], argv[2], exec_cmd);
  exit(0);
}