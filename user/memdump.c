#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void memdump(char *fmt, char *data)
{
  int i;
  char *str;

  for (i = 0; fmt[i]; i++)
  {
    switch (fmt[i])
    {
    case 'i':
      printf("%d ", *(int *)data);
      data += 4;
      break;
    case 'p':
      printf("0x%lx ", *(uint64 *)data);
      data += 8;
      break;
    case 'h':
      printf("%d ", *(short *)data);
      data += 2;
      break;
    case 'c':
      printf("%c ", *data);
      data += 1;
      break;
    case 's':
      str = *(char **)data;
      printf("%s ", str);
      data += 8;
      break;
    case 'S':
      printf("%s ", data);
      while (*data)
        data++;
      data++;
      break;
    }
  }
  printf("\n");
}

int main(int argc, char *argv[])
{
  char buf[512];
  char test_data[] = "Hello World!";
  char *test_str = test_data;

  if (argc == 1)
  {
    printf("memdump examples:\n");

    int test_int = 42;
    short test_short = 123;
    char test_char = 'A';
    uint64 test_ptr = 0x1234abcd;

    char example_data[32];
    *(int *)example_data = test_int;
    *(short *)(example_data + 4) = test_short;
    *(example_data + 6) = test_char;
    *(uint64 *)(example_data + 8) = test_ptr;
    *(char **)(example_data + 16) = test_str;
    strcpy(example_data + 24, "test");

    printf("Format 'i': ");
    memdump("i", example_data);
    printf("Format 'h': ");
    memdump("h", example_data + 4);
    printf("Format 'c': ");
    memdump("c", example_data + 6);
    printf("Format 'p': ");
    memdump("p", example_data + 8);
    printf("Format 's': ");
    memdump("s", example_data + 16);
    printf("Format 'S': ");
    memdump("S", example_data + 24);

    exit(0);
  }

  if (argc == 2)
  {
    int n;
    while ((n = read(0, buf, sizeof(buf))) > 0)
      memdump(argv[1], buf);
  }
  else
    memdump(argv[1], argv[2]);

  exit(0);
}