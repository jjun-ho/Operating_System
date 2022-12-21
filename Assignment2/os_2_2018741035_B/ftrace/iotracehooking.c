#include "ftracehooking.h"

#define __NR_open 2
#define __NR_read 0
#define __NR_write 1
#define __NR_lseek 8
#define __NR_close 3

extern void make_rw(void *addr);
extern void make_ro(void *addr);

void **syscall_table;

unsigned int read_bytes = 0;
unsigned int write_bytes =0;

unsigned int open_count = 0;
unsigned int close_count = 0;
unsigned int read_count = 0;
unsigned int write_count = 0;
unsigned int lseek_count = 0;

asmlinkage long(*real_open)(const char*, int, umode_t);
asmlinkage long(*real_close)(unsigned int);
asmlinkage long(*real_read)(unsigned int, char*, size_t);
asmlinkage long(*real_write)(unsigned int, const char*, size_t);
asmlinkage long(*real_lseek)(unsigned int, off_t, unsigned int);
 
asmlinkage long ftrace_open(const char __user *filename, int flags, umode_t mode)
{
	open_count += 1;
	return(real_open(filename, flags, mode));
}
asmlinkage long ftrace_close(unsigned int fd)
{
	close_count += 1;
	return(real_close(fd));
}
asmlinkage long ftrace_read(unsigned int fd, char __user *buf, size_t count)
{
	read_count += 1;
	read_bytes = sizeof(real_read(fd, buf, count));
	return(real_read(fd, buf, count));
}
asmlinkage long ftrace_write(unsigned int fd, const char __user *buf, size_t count)
{
	write_count += 1;
	write_bytes = sizeof(real_write(fd, buf, count));
	return(real_write(fd, buf, count));
}
asmlinkage long ftrace_lseek(unsigned int fd, off_t offset, unsigned int whence)
{
	lseek_count += 1;
	return(real_lseek(fd, offset, whence));
}


static int __init iotracehooking_init(void)
{
	/* Find system call table */
	syscall_table = (void**) kallsyms_lookup_name("sys_call_table");

	/*
	 * change permission of the page of system call table
	 * to both readable and writable
	 */
	make_rw(syscall_table);
	real_open = syscall_table[__NR_open];
	real_close = syscall_table[__NR_close];
	real_read = syscall_table[__NR_read];
	real_write = syscall_table[__NR_write];
 	real_lseek = syscall_table[__NR_lseek];
	syscall_table[__NR_open] = ftrace_open;
	syscall_table[__NR_close] = ftrace_close;
	syscall_table[__NR_read] = ftrace_read;
	syscall_table[__NR_write] = ftrace_write;
	syscall_table[__NR_lseek] = ftrace_lseek;

	return 0;
}


static void __exit iotracehooking_exit(void)
{
	  printk(KERN_INFO "[2018741035] ./test file[abc.txt] stats [x] read - %d / written - %d\n ", read_bytes, write_bytes);

        printk(KERN_INFO "open[%d] close[%d] read[%d] write[%d] lseek[%d]\n", open_count, close_count, read_count, write_count, lseek_count);

	syscall_table[__NR_open] = real_open;
	syscall_table[__NR_close] = real_close;
	syscall_table[__NR_read] = real_read;
	syscall_table[__NR_write] = real_write;
	syscall_table[__NR_lseek] = real_lseek;
	
	read_bytes = 0;
	write_bytes = 0;
	
	open_count = 0;
	close_count = 0;
	read_count = 0;
	write_count = 0;
	lseek_count = 0;
	
	/* Recover the page's permission (i.e. read-only)*/
	make_ro(syscall_table);
}

module_init(iotracehooking_init);
module_exit(iotracehooking_exit);
MODULE_LICENSE("GPL");

