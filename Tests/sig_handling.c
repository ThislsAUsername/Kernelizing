// these are methods for handling various interrupts, they will detect
// signals other than SIGKILL and SIGSTOP

// note to others, typically .c files should not be included,
// but it makes life easier here so we will do so

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void sig_handler(int signo)
{
    if (signo == SIGTERM)
        printf("received SIGTERM\n");
    else if (signo == SIGINT)
        printf("received SIGINT\n");
    else if (signo == SIGQUIT)
        printf("received SIGQUIT\n");
    else if (signo == SIGKILL)
        printf("received SIGKILL\n");
    else if (signo == SIGSTOP)
        printf("received SIGSTOP\n");
    else if (signo == SIGHUP)
        printf("received SIGHUP\n");

    exit(0); // we catch signals so they can be reported, but we still quit
}

void register_handlers(void)
{
    if (signal(SIGTERM, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGTERM\n");
    if (signal(SIGINT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGINT\n");
    if (signal(SIGQUIT, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGQUIT\n");
    //if (signal(SIGKILL, sig_handler) == SIG_ERR)
    //    printf("\ncan't catch SIGKILL\n");
    //if (signal(SIGSTOP, sig_handler) == SIG_ERR)
    //    printf("\ncan't catch SIGSTOP\n");
    if (signal(SIGHUP, sig_handler) == SIG_ERR)
        printf("\ncan't catch SIGHUP\n");

}
