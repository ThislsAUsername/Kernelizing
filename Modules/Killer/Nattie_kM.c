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

struct task_struct *task;
static typedef struct ProcessShell {
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
}*head = NULL;
pid_t innocents[MAX_INNOCENTS];
int currentInnocents = 0;
int data;

static int threadThing(void* input){
	for_each_process(t)
	{
		if (currentInnocents < MAX_INNOCENTS)
		{
			innocents[currentInnocents] = t->pid;
		}
		currentInnocents++;
	}
	while (!kthread_should_stop())
	{
		for_each_process(t)
		{
			bool isInnocent = false; // guilty until proven innocent
			for (int i = 0; i < currentInnocents && !isInnocent; i++)
			{
				if (t->pid == innocents[i])
				{
					isInnocent = true;
				}
			}
			if (!isInnocent)
			{
				ProcessShell* newShell = kmalloc(sizeof());
				newShell->pid = t->pid;
				//TIME stuff
				cputime_t utime;
				cputime_t stime;
				cputime_t jiffies;
				//convert from jiffies to sec
				utime = 0;
				stime = 0;
				thread_group_cputime_adjusted(t, &utime, &stime); //from blackboard FAQ
				jiffies = utime + stime;
				newShell->seconds = jiffies / HZ;
				//convert from sec to hr-min-sec
				newShell->hour = newShell->seconds / (60 * 60);
				newShell->seconds = newShell->seconds % (60 * 60);
				newShell->minutes = newShell->seconds / 60;
				newShell->seconds = newShell->seconds % 60;
			}
		}

		//The parent kiddo add stuff and thingy...
		int highestChildCount = 0;
		pid_t highestChildPID;
		boolean allPropogated = false;
		int countProps = 0;
		int linkedLength = 0;
		ProcessShell* temp = head;
		ProcessShell* bigDaddy = NULL;

		//Finds the size of linked list (# of nodes)
		while (temp->next != NULL)
		{
			linkedLength++;
			temp = temp->next;
		}
		temp = head;

		//does the thing
		while(allPropogated != true)
		{
			if (temp->propogated != true)
			{
				//If there's a parent add the kid count to it and propogate.
				if (temp->parent != NULL)
				{
					bigDaddy = temp->parent;
					bigDaddy->childrenCount += temp->childrenCount;
					bigDaddy->propogated = false;
					temp->propogated = true;
					temp->childrenCount = 0;
					temp->parent = bigDaddy;

				}
				else
				{
					temp->propogated = true;
				}

				//move to next node, if at end, go back to beginning
				if (temp->next != NULL)
				{
					temp = temp->next;
				}
				else
				{
					temp = head;
					countProps = 0;
				}
			}
			//if looking at something already propogated, move to the next thing and countProps++.
			else
			{
				if (temp->next != NULL)
				{
					temp = temp->next;
					countProps++;
					if (countProps >= linkedLength)
					{
						allPropogated = true;
					}
				}
				else
				{
					temp = head;
					countProps = 0;
				}
			}
		}

		//goes through the list and finds the highestChildCount and the PID of it.
		temp = head;
		while(temp != NULL)
		{
			if (temp->childrenCount > highestChildCount)
			{
				highestChildCount = temp->childrenCount;
				highestChildPID = temp->pid;
			}

			temp = temp->next;
		}
		//end off add kid count to bigDaddy stuff...

		schedule();
		//kill(pid, SIGKILL);
	}
	return 0;
}

static void searchPID(pid_t searchFor)
{
	ProcessShell* temp = NULL;
	temp->next = head;
	while (temp->next != NULL)
	{
		if (temp.pid == searchFor)
		{
			return temp;
			break;
		}
		else
		{
			temp = temp->next;
		}
	}
	return NULL;
}

static void deleteList()
{
	ProcessShell* temp = NULL;
	ProcessShell* tempNext = NULL;

	temp = head;
	while (temp != NULL)
	{
		tempNext = temp->next;
		kfree(temp);
		temp = tempNext;
	}
}

static 

static int __init my_name(void)
{
	printk(KERN_INFO "Josiah White\n");
	task = kthread_run(&threadThing, (void *)data, "fork-bomb-killer");
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
