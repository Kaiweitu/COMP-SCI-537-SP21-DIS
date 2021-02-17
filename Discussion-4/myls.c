#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(void) {
    pid_t rnt = fork();
    if (rnt == -1) {
        perror("parent process: fork failed");
        exit(1);
    }
    if (rnt == 0) {
        // child process goes here
        printf("child: start executing ls -l\n");
        
        // TODO: Redirection 

        // First argument by convention should be the full path to the program
        // Last argument should be NULL pointer
        // TODO: execv 

        // if child reached here, exec failed
        printf("child process: exec failed\n");
        _exit(1);
    } else {
        // parent process goes here
        int wstatus;
        wait(&wstatus);
        printf("parent process: child process exits\n");
    }

    return 0;
}