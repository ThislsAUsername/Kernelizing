//http://www.makelinux.net/ldd3/chp-5-sect-3
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/kthread.h>
#include <linux/sched.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/semaphore.h>

#define MAX_INNOCENTS 512
#define KILL_THRESHOLD 42

static struct task_struct *task;
static struct task_struct *t;
typedef struct ProcessShell {
	bool propogated;
	int childrenCount;
	int hour;
	int minutes;
	int seconds;
	pid_t pid;

	char* tty[16];
	char* cmd[32];
	struct ProcessShell* parent;
	struct ProcessShell* next;
} uselesStruct;
struct ProcessShell *head;
static int data;

static int threadThing(void* input) {
return 0;
}

static int __init my_name(void)
{
	printk(KERN_INFO "Josiah White\n");
	//task = kthread_run(&threadThing,(void *)data,"fork-bomb-killer");
	task = kthread_run(&threadThing,(void *)data,"fork-bomb-killer");
	return 0;
}

static void __exit my_exit(void)
{
	kthread_stop(task);
}

module_init(my_name);
module_exit(my_exit);
MODULE_AUTHOR("Josiah");
MODULE_LICENSE("GPL");
