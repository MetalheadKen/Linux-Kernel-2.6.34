#include <linux/kernel.h>

/*
 * Show University Student ID
 */
asmlinkage long sys_show_usid(void)
{
    printk(KERN_EMERG "ntust_m10702130");
    return 0;
}
