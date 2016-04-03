/*
 * fork_binary.c
 *
 *  Created on: Dec 25, 2015
 *      Author: duolu
 */

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "sig_handling.c"



int main(int argc, char **argv)
{
	int i;
	pid_t pid;

	if(argc < 2) {
		printf("Usage: fork_binary n\n");
		return 0;
	}

	int n = strtol(argv[1], NULL, 10);

        register_handlers();

	int p = getpid();
	printf("%d\n", p);
	fflush(stdout);

	char new_n[4];
	snprintf(new_n, sizeof(new_n), "%d", n - 1);	

	if(n > 0) {

    		pid = fork();
    		if(pid == 0){
    			execl(argv[0], argv[0], new_n, NULL);
    		}
    		pid = fork();
    		if(pid == 0){
    			execl(argv[0], argv[0], new_n, NULL);
    		}
	}
 
    	while(1)
    		;
	return 0;
}

