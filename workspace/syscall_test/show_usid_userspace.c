#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

int main(void)
{
    long retval = syscall(338);
    return 0;
}
