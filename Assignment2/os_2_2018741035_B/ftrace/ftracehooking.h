#include <linux/kernel.h>
#include <linux/syscalls.h> /* __SYSCALL_DFINRx() */

#include <linux/module.h>
#include <linux/highmem.h>	
#include <linux/kallsyms.h> /* kallstms_lookup_name() */
#include <asm/syscall_wrapper.h> /* __SYSCALL_DFIRx() */

#include <asm/uaccess.h>

#include <linux/sched.h>
#include <linux/init_task.h>
