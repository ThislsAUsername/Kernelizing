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

#define MAX_INNOCENTS 512

struct task_struct *task;
typedef struct ProcessShell {
	bool propogated;
	int childrenCount;
	int hr;
	int min;
	int sec;
	pid_t pid;
	
	char* tty[16];
	char* cmd[32];
	struct ProcessShell* parent;
}* head;
pid_t innocents[MAX_INNOCENTS];
int data;

static int threadThing(void* input){
	int i = 0;	
	for_each_process(t)
	{
		if (i < MAX_INNOCENTS)
		{
			innocents[i] = t->pid;
		}
		i++;
	}
	while (!kthread_should_stop()){
		schedule();
		
	}
	return 0;
}

static int __init my_name(void)
{
	printk(KERN_INFO "Josiah White\n");
	task = kthread_run(&threadThing,(void *)data,"pradeep");
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
