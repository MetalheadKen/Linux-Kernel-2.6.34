#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/list.h>
#include <linux/cred.h>
#include <linux/string.h>
#include <asm-generic/cputime.h>
#include "proc_info.h"

/*
 * Collect information of the current process
 */
SYSCALL_DEFINE1(proc_info, struct prinfo *, proc)
{
    struct prinfo proc_info;
    struct task_struct *task, *curr_task;
    struct list_head *list;

    unsigned long start_time, diff_time_1, diff_time_2, sibling_start_time;
   
    curr_task = current;

    memset(&proc_info, 0, sizeof(proc_info));

    proc_info.state = curr_task->state;                 /* Get current state of process */
    proc_info.nice  = TASK_NICE(curr_task);             /* Get nice value of process */

    proc_info.pid   = task_tgid_vnr(curr_task);         /* Get current PID of process */
    proc_info.parent_pid = curr_task->parent->pid;      /* Get parent PID of process */

    proc_info.start_time = timespec_to_ns(&curr_task->start_time);      /* Get process start time */
    proc_info.user_time  = curr_task->utime;                            /* Get CPU time spent in user mode */
    proc_info.sys_time   = curr_task->stime;                            /* Get CPU time spent in system mode */

    proc_info.uid   = current_uid();                    /* Get user id of process owner */
    strcpy(proc_info.comm, curr_task->comm);            /* Get name of program executed */

    /* 
     * Get youngest children PID of process
     * The youngest children is the process in the children list with highest start time
     */
    proc_info.youngest_child_pid = start_time = NONE;

    list_for_each(list, &(curr_task->children)) {
        task = list_entry(list, struct task_struct, sibling);

        /* Get total user time and total system time of children */
        proc_info.cutime = cputime_add(proc_info.cutime, task->utime);
        proc_info.cstime = cputime_add(proc_info.cstime, task->stime);

        if (start_time < timespec_to_ns(&task->start_time)) {
            start_time = timespec_to_ns(&task->start_time);
            proc_info.youngest_child_pid = task->pid;
        }
    }

    /*
     * Get younger and older sibling PID of process
     * The next younger sibling is the process in the siblings list with a start time that is larger than,
     * but nearest to the start time of the current process
     * Likewise, the next older sibling is the one in the list with a next smaller start time
     */
    diff_time_1 = diff_time_2 = LONG_MAX;
    proc_info.younger_sibling_pid = proc_info.older_sibling_pid = NONE;

    list_for_each(list, &(curr_task->sibling)) {
        task = list_entry(list, struct task_struct, sibling);

        sibling_start_time = timespec_to_ns(&task->start_time);

        if (proc_info.start_time < sibling_start_time && diff_time_1 > sibling_start_time - proc_info.start_time) {
            diff_time_1 = sibling_start_time - proc_info.start_time;
            proc_info.younger_sibling_pid = task->pid;
        }

        if (proc_info.start_time > sibling_start_time && diff_time_2 > proc_info.start_time - sibling_start_time) {
            diff_time_2 = proc_info.start_time - sibling_start_time;
            proc_info.older_sibling_pid = task->pid;
        }
    }
    
    if (copy_to_user(proc, &proc_info, sizeof(proc_info)))
        return -EFAULT;

    return 0;
}

