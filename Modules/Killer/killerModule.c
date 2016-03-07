//http://www.makelinux.net/ldd3/chp-5-sect-3
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/sched.h>
#include <linux/kthread.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/tty.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/types.h>
#include <linux/signal.h>

#define MAX_INNOCENTS 512
#define KILL_THRESHOLD 42

static struct task_struct *task;
static struct task_struct *t;
typedef struct ProcessShell
{
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
static pid_t innocents[MAX_INNOCENTS];
static pid_t targets[MAX_INNOCENTS];
static int currentInnocents;
static int currentTargets;
static int data;


void thread_group_cputime_adjusted(struct task_struct *t, cputime_t *utime, cputime_t *stime)
{
    static struct task_struct *child;
    for_each_thread(t,child)
    {
        *utime += child->utime;
        *stime += child->stime;
    }
}

static struct ProcessShell* searchPID(pid_t searchFor)
{
    struct ProcessShell* temp = NULL;
    temp->next = head;
    while (temp->next != NULL)
    {
        if (temp->pid == searchFor)
        {
            return temp;
        }
        else
        {
            temp = temp->next;
        }
    }
    return NULL;
}

static void deleteList(void)
{
    struct ProcessShell* temp = NULL;
    struct ProcessShell* tempNext = NULL;

    temp = head;
    while (temp != NULL)
    {
        tempNext = temp->next;
        kfree(temp);
        temp = tempNext;
    }
}

static int threadThing(void* input)
{
    int countProps = 0;
    int linkedLength = 0;
    int highestChildCount;
    cputime_t jiffies;
    struct ProcessShell* wearerofpants = NULL;
    pid_t highestChildPID;
    bool allPropogated = false;
    struct ProcessShell* temp = head;
    struct ProcessShell* bigDaddy = NULL;
    currentInnocents = 0;
    currentTargets = 0;
    highestChildCount = 0;
    head = NULL;
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
        // collecting processes
        for_each_process(t)
        {
            bool isInnocent = false; // guilty until proven innocent
            int i;
            for (i = 0; i < currentInnocents && !isInnocent; i++)
            {
                if (t->pid == innocents[i])
                {
                    isInnocent = true;
                }
            }
            if (!isInnocent)
            {
                struct ProcessShell* newShell = kmalloc(sizeof(struct ProcessShell), GFP_KERNEL);
                cputime_t utime = 0;
                cputime_t stime = 0;
                // basics
                newShell->propogated = false;
                newShell->childrenCount = 0;
                newShell->next = head;
                // PID
                newShell->pid = t->pid;
                //TIME
                thread_group_cputime_adjusted(t, &utime, &stime); //from blackboard FAQ
                jiffies = utime + stime;
                //convert from jiffies to sec
                newShell->seconds = jiffies / HZ;
                //convert from sec to hr-min-sec
                newShell->hour = newShell->seconds/(60*60);
                newShell->seconds = newShell->seconds%(60*60);
                newShell->minutes = newShell->seconds/60;
                newShell->seconds = newShell->seconds%60;
                // PARENTING
                newShell->parent = searchPID(t->real_parent->pid);
            }
        }
        // account for direct children
        wearerofpants = head;
        while (wearerofpants != NULL)
        {
            if (wearerofpants->parent != NULL)
            {
                wearerofpants->parent->childrenCount++;
            }
            wearerofpants = wearerofpants->next;
        }
        //The parent kiddo add stuff and thingy...

        //Finds the size of linked list (# of nodes)
        temp = head;
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
        // KILL THEM ALL
        if (highestChildCount >= KILL_THRESHOLD)
        {
            int checkedFamilies;
            int i;
            struct ProcessShell *checking = searchPID(highestChildPID);
            targets[0] = checking->pid;
            checkedFamilies = 0;
            currentTargets = 1; // fill the array after the mother process
            while (checkedFamilies < currentTargets)
            {
                wearerofpants = head;
                while (wearerofpants != NULL)
                {
                    if (wearerofpants->parent == checking)
                    {
                        targets[currentTargets] = wearerofpants->pid;
                        currentTargets++;
                    }
                }
                checkedFamilies++;
            }
            for (i = 0; i < currentTargets; i++)
            {
                /*
                struct siginfo info;

                memset(&info, 0, sizeof(info));
                info.si_signo = SIGKILL;
                //info.si_code = SI_QUEUE;
                //info.si_addr = NULL;
                //kill_proc_info(SIGKILL,&info,find_vpid(targets[i]));
                for_each_process(t)
                {
                    if (targets[i] == t->pid)
                    {
                        send_sig_info(SIGKILL, &info, t);
                    }
                }*/
                for_each_process(t)
                {
                    if (targets[i] == t->pid)
                    {
                        force_sig(SIGKILL, t);
                    }
                }
                /*for_each_process(t)
                {
                    if (targets[i] == t->pid)
                    {
                        send_sig(SIGKILL,t,0);
                    }
                }*/
                /*for_each_process(t)
                {
                    if (targets[i] == t->pid)
                    {
                        rcu_read_lock();
                        kill_pid(t, SIGKILL, find_vpid(targets[i]));
                        rcu_read_unlock();
                    }
                }*/
            }
        }
        deleteList();
        head = NULL;
//        set_current_state(TASK_INTERRUPTIBLE);
        schedule();
    }
    return 0;
}

static int __init my_name(void)
{
    printk(KERN_INFO "Josiah White\n");
    //task = kthread_run(&threadThing,(void *)data,"fork-bomb-killer");
    task = kthread_run(&threadThing,(void *)data,"badass");
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
