#ifndef __PROC_INFO_H__
#define __PROC_INFO_H__

#include <linux/types.h>

#define NONE -1

/*
 * Convert user-nice values [ -20 ... 0 ... 19 ]
 * to static priority [ MAX_RT_PRIO..MAX_PRIO-1 ],
 * and back.
 */
#define NICE_TO_PRIO(nice)	(MAX_RT_PRIO + (nice) + 20)
#define PRIO_TO_NICE(prio)	((prio) - MAX_RT_PRIO - 20)
#define TASK_NICE(p)		PRIO_TO_NICE((p)->static_prio)

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

#endif /* __PROC_INFO_H__ */

