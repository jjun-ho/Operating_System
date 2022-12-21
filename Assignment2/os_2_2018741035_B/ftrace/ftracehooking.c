#include "ftracehooking.h"

#define __NR_ftrace 336

void **syscall_table;

unsigned int pid_n = 0;

asmlinkage int(*real_ftrace)(pid_t);
asmlinkage int my_ftrace(pid_t pid)
{
	return (real_ftrace(pid));
}

void make_rw(void *addr)
{
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);
	if(pte->pte &~ _PAGE_RW)
		pte->pte |= _PAGE_RW;
}

void make_ro(void *addr)
{
	unsigned int level;
	pte_t *pte = lookup_address((u64)addr, &level);
	
	pte->pte = pte->pte &~ _PAGE_RW;
}


static int __init ftracehooking_init(void)
{
	struct task_struct *task = current;
	pid_n = task->pid;
	printk(KERN_INFO "OS Assignment 2 ftrace [%d] Start\n", pid_n);
	
	/* Find system call table */
	syscall_table = (void**) kallsyms_lookup_name("sys_call_table");

	/*
	 * change permission of the page of system call table
	 * to both readable and writable
	 */
	make_rw(syscall_table);
	real_ftrace = syscall_table[__NR_ftrace];
	syscall_table[__NR_ftrace] = my_ftrace;

	return 0;
}


static void __exit ftracehooking_exit(void)
{	
	printk(KERN_INFO "OS Assignment 2 ftrace [%d] End\n", pid_n);

	syscall_table[__NR_ftrace] = real_ftrace;

	/* Recover the page's permission (i.e. read-only)*/	
	make_ro(syscall_table);	
}

module_init(ftracehooking_init);
module_exit(ftracehooking_exit);
MODULE_LICENSE("GPL");

EXPORT_SYMBOL(make_rw);
EXPORT_SYMBOL(make_ro);
	
