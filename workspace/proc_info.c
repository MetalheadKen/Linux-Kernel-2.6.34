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
    struct task_struct *curr_task, *sibling;
   
    curr_task = current;

    proc_info.state = curr_task->state;         /* Get current state of process */
    proc_info.nice  = TASK_NICE(curr_task);     /* Get nice value of process */

    proc_info.pid   = task_tgid_vnr(curr_task);              /* Get current PID of process */
    proc_info.parent_pid = curr_task->real_parent->pid;     /* Get parent PID of process */

    /* 
     * Get youngest children PID of process
     * Check if current process has any childrens
     */
    if (!list_empty(&curr_task->children))
        proc_info.youngest_child_pid = (list_first_entry(&curr_task->children, struct task_struct, children))->pid;
    else
        proc_info.youngest_child_pid = NONE;

    /*
     * Get younger sibling PID of process
     * Check if current process has any younger siblings
     */
    sibling = list_entry(curr_task->sibling.next, struct task_struct, sibling);

    if (sibling->pid < curr_task->pid)
        proc_info.younger_sibling_pid = sibling->pid;
    else
        proc_info.younger_sibling_pid = NONE;
    
    /*
     * Get older sibling PID of process
     * Check if current process has any older siblings
     */
    sibling = list_entry(curr_task->sibling.prev, struct task_struct, sibling);

    if (sibling->pid < curr_task->pid)
        proc_info.older_sibling_pid = sibling->pid;
    else
        proc_info.older_sibling_pid = NONE;

    proc_info.start_time = timespec_to_ns(&curr_task->start_time);      /* Get process start time */
    proc_info.user_time  = curr_task->utime;                            /* Get CPU time spent in user mode */
    proc_info.sys_time   = curr_task->stime;                            /* Get CPU time spent in system mode */

    /*
     * Get total user time and total system time of children
     * Check if current process has any childrens
     */
    proc_info.cutime = proc_info.cstime = 0;

    if (!list_empty(&curr_task->children)) {
        struct list_head *pos;

        list_for_each(pos, &curr_task->children) {
            struct task_struct *children = list_entry(pos, struct task_struct, children);

            proc_info.cutime = cputime_add(proc_info.cutime, children->utime);
            proc_info.cstime = cputime_add(proc_info.cstime, children->stime);
        }
    }
    
    proc_info.uid = current_uid();                  /* Get user id of process owner */
    strcpy(proc_info.comm, curr_task->comm);        /* Get name of program executed */

    if (copy_to_user(proc, &proc_info, sizeof(proc_info)))
        return -EFAULT;

    return 0;
}

