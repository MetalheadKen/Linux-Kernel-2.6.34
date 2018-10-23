#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>

struct prinfo {
    long state;          /* current state of process */
    long nice;           /* process nice value */

    pid_t pid;                  /* process id */
    pid_t parent_pid;           /* process id of parent */
    pid_t youngest_child_pid;   /* pid of youngest child */
    pid_t younger_sibling_pid;  /* pid of younger sibling */
    pid_t older_sibling_pid;    /* pid of older sibling */

    unsigned long start_time;   /* process start time */
    long user_time;             /* CPU time spent in user mode */
    long sys_time;              /* CPU time spent in system mode */
    long cutime;                /* total user time of children */
    long cstime;                /* total system time of children */

    long uid;            /* user id of process owner */
    char comm[16];       /* name of program executed */
};

void delay(void);
void print_proc_info(void);

int main(void)
{
    int pid_1, pid_2, wstatus;

    pid_1 = fork();

    if (pid_1 < 0) {
        printf("Fork failed.\n");
    } else if (pid_1 == 0) {
        /* child */
        delay();
    }

    /* parent */
    pid_2 = fork();

    if (pid_2 < 0) {
        printf("Fork failed.\n");
    } else if (pid_2 == 0) {
        delay();
    }

    print_proc_info();

    waitpid(-1, &wstatus, 0);
    waitpid(-1, &wstatus, 0);
}

void delay(void)
{
    for (int i = 0; i < 100000000; i++) ;
}

void print_proc_info(void)
{
    struct prinfo proc_info;

    if (syscall(339, &proc_info) > 0) {
        printf("System call failed\n");
        return;
    }
    printf("\n");

    printf("state               = %ld\n", proc_info.state);
    printf("nice                = %ld\n", proc_info.nice);

    printf("pid                 = %d\n",  proc_info.pid);
    printf("parent pid          = %d\n",  proc_info.parent_pid);
    printf("youngest child pid  = %d\n",  proc_info.youngest_child_pid);
    printf("younger sibling pid = %d\n",  proc_info.younger_sibling_pid);
    printf("older sibling pid   = %d\n",  proc_info.older_sibling_pid);
    
    printf("start time          = %lu\n", proc_info.start_time);
    printf("user time           = %ld\n", proc_info.user_time);
    printf("sys time            = %ld\n", proc_info.sys_time);
    printf("cutime              = %ld\n", proc_info.cutime);
    printf("cstime              = %ld\n", proc_info.cstime);

    printf("uid                 = %ld\n", proc_info.uid);
    printf("comm                = %s\n",  proc_info.comm);
}

