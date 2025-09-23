// Copyright (c) Kabeers Network Authors

#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[])
{
  char buf[512];
  char num_str[16];
  int fd, bytes_read, i, j;
  int num;

  if (argc != 2)
  {
    printf("Usage: sixfive file\n");
    exit(1);
  }

  fd = open(argv[1], 0);
  if (fd < 0)
  {
    printf("cannot open file\n");
    exit(1);
  }

  while ((bytes_read = read(fd, buf, 512)) > 0)
  {
    i = 0;
    while (i < bytes_read)
    {
      if (buf[i] >= '0' && buf[i] <= '9')
      {
        j = 0;
        while (i < bytes_read && buf[i] >= '0' && buf[i] <= '9')
        {
          num_str[j] = buf[i];
          i++;
          j++;
        }
        num_str[j] = 0;

        num = atoi(num_str);
        if (num % 5 == 0 || num % 6 == 0)
        {
          printf("%d\n", num);
        }
      }
      else
      {
        i++;
      }
    }
  }

  close(fd);
  exit(0);
}