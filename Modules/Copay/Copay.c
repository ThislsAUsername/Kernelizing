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
#define KILL_THRESHOLD 13
#define DEBAHG_PREENT(x); ;

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
    pid_t pidParent;

    char* tty[16];
    char* cmd[32];
    struct ProcessShell* shellParent;
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
    DEBAHG_PREENT(KERN_INFO "timing start\n");
    for_each_thread(t,child)
    {
        *utime += child->utime;
        *stime += child->stime;
    }
    DEBAHG_PREENT(KERN_INFO "timing end\n");
}

static struct ProcessShell* searchPID(pid_t searchFor)
{
    struct ProcessShell placeholder;
    struct ProcessShell* temp = &placeholder;
    DEBAHG_PREENT(KERN_INFO "beginning a search\n");
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
    int highestChildCount;
    cputime_t jiffies;
    struct ProcessShell* currentShell = NULL;
    pid_t highestChildPID;
    bool allPropogated = false;
    struct ProcessShell* temp = head;
    struct ProcessShell* bigDaddy = NULL;
    currentInnocents = 0;
    currentTargets = 0;
    highestChildCount = 0;
    head = NULL;
    DEBAHG_PREENT(KERN_INFO "populating innocents\n");
    for_each_process(t)
    {
        if (currentInnocents < MAX_INNOCENTS)
        {
            innocents[currentInnocents] = t->pid;
        }
        currentInnocents++;
    }
    DEBAHG_PREENT(KERN_INFO "entering loop\n");
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
                DEBAHG_PREENT(KERN_INFO "adding to list\n");
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
                DEBAHG_PREENT(KERN_INFO "successfully time'd\n");
                // PARENTING
                if (t->real_parent != NULL)
                {
                    DEBAHG_PREENT(KERN_INFO "iz real boi\n");
                    newShell->pidParent = t->real_parent->pid;
                }
                else
                {
                    DEBAHG_PREENT(KERN_INFO "iz pinoccio\n");
                    newShell->shellParent = 0;
                }
                head = newShell;
                DEBAHG_PREENT(KERN_INFO "successfully added\n");
            }
        }
        // populate shellParents
        currentShell = head;
        while (currentShell != NULL)
        {
            currentShell->shellParent = searchPID(currentShell->pidParent);
            currentShell = currentShell->next;
            DEBAHG_PREENT(KERN_INFO "shellParent populated\n");
        }
        // account for direct children
        currentShell = head;
        while (currentShell != NULL)
        {
            if (currentShell->shellParent != NULL)
            {
                currentShell->shellParent->childrenCount++;
            }
            currentShell = currentShell->next;
        }
        DEBAHG_PREENT(KERN_INFO "direct parents done\n");

        if (head != NULL)
        {
            //The parent kiddo add stuff and thingy...
            allPropogated = false;
            DEBAHG_PREENT(KERN_INFO "list isn't null\n");
            //does the thing
            while(allPropogated != true)
            {
                temp = head;
                allPropogated = true; // start optimistic
                highestChildCount = 0;
                while (temp != NULL)
                {
                    if (temp->propogated != true)
                    {
                        allPropogated = false;
                        //If there's a parent add the kid count to it and propogate.
                        if (temp->shellParent != NULL)
                        {
                            bigDaddy = temp->shellParent;
                            bigDaddy->childrenCount += temp->childrenCount;
                            bigDaddy->propogated = false;
                            temp->propogated = true;
                            temp->childrenCount = 0;
                            //temp->shellParent = bigDaddy;

                        }
                        else
                        {
                            temp->propogated = true;
                        }
                    }
                    //move to next node
                    temp = temp->next;
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
        }
        // KILL THEM ALL
        printk(KERN_INFO "We have %d children\n", highestChildCount);
        if (highestChildCount >= KILL_THRESHOLD)
        {
            int checkedFamilies;
            int i;
            struct ProcessShell *checking;
            printk(KERN_INFO "Searching\n");
            checking = searchPID(highestChildPID);
            targets[0] = checking->pid;
            checkedFamilies = 0;
            currentTargets = 1; // fill the array after the mother process
            DEBAHG_PREENT(KERN_INFO "murdertime! <3\n");
            while (checkedFamilies < currentTargets)
            {
                currentShell = head;
                while (currentShell != NULL)
                {
                    if (currentShell->shellParent == checking)
                    {
                        targets[currentTargets] = currentShell->pid;
                        currentTargets++;
                    }
                    currentShell = currentShell->next;
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
    DEBAHG_PREENT(KERN_INFO "starting kthread\n");
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
