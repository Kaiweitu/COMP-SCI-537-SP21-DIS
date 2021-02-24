#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define BUFFER_SIZE 512
#define MAX_ARGUMENT 512

void prompt() {
    write(STDOUT_FILENO, "myparser> ", 10);
}


void parse(char *str) {
    char *c;
    char *output_file = NULL;
    char *argv[MAX_ARGUMENT];
    int args = 0;

    // TODO: find the redirection symbol

    char *arg;

    // TODO: fetch the tokens

    printf("Tokens:\n");
    fflush(stdout);
    for (int i = 0; i < args; i++) {
        write(STDOUT_FILENO, argv[i], strlen(argv[i]));
        write(STDOUT_FILENO, "\n", 1);
    }

    if (output_file != NULL) {
        printf("Redirect to file [%s]\n", output_file);
        fflush(stdout);
    }
        
}

int main(int argc, char **argv) {
    char buffer[BUFFER_SIZE];

    prompt();
    while (fgets(buffer, BUFFER_SIZE, stdin)) {
        // write(STDOUT_FILENO, buffer, strlen(buffer));
        parse(buffer);
        prompt();
    }
}