/*
 * fork_loop.c
 *
 *  Created on: Dec 25, 2015
 *      Author: duolu
 */

#include <stdio.h>

#include <unistd.h>




int main(int argc, char **argv)
{
	int i;
	pid_t pid;

	if(argc < 2) {
		printf("Usage: fork_loop n\n");
		return 0;
	}

	int n = strtol(argv[1], NULL, 10);

    	for(i = 0; i < n; i++) {

    		pid = fork();
    		if(pid == 0){
    			break;
    		} else {
			printf("%d\n", pid);
			fflush(stdout);

		}
    	}

    	while(1)
    		;
	return 0;
}

