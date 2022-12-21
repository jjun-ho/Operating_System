#include <linux/kernel.h>
#include <linux/syscalls.h>

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/init_task.h>

SYSCALL_DEFINE1(ftrace, pid_t, pid)
{
	return pid;
}
