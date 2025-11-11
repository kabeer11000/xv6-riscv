
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(void)
{
    printf ("syscall counts before: %d\n ", getsyscallcount());

    getpid();
    uptime();
    uptime();

    printf("syscall counts after: %d\n ", getsyscallcount());

    exit(0);
}