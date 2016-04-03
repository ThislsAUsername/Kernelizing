/*
 * fork_loop.c
 *
 *  Created on: Dec 25, 2015
 *      Author: duolu
 *
 * This version has children that die on their own after spawning, 
 * they should not be killed
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "sig_handling.c"


int main(int argc, char **argv)
{
	int i;
	pid_t pid;

	if(argc < 2) {
		printf("Usage: fork_safe n\n");
		return 0;
	}

        register_handlers();

	int n = strtol(argv[1], NULL, 10);

    for(i = 0; i < n; i++) {
        pid = fork();
        if(pid == 0){
            usleep(10000);
            exit(0);
        } else if (pid == -1) {
            printf("Unable to allocate any more processes, allocated %d\n", i);
            return 0;
        } else {
            printf("child pid %d, killing...\n", pid);
            usleep(10000);
            waitpid(pid, NULL, 0); // reap child.  Else it persists as a zombie
        }
    }
        usleep(1000000); // wait a second to see if we get killex
	return 0;
}

