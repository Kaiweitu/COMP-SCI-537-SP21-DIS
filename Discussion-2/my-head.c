/**
 * Simple my-head implementation
 *
 * Usage: ./my-head [-n num] filename
 *
 * Copyright Kaiwei Tu
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE 128
static void exit_with_error() {
    printf("my-head: invalid command line\n");
    exit(1);
}

int main(int argc, char* argv[]) {
    // Fill out the code for parsing the argument here
    int opt, num_lines = 10;
    
    while ((opt = getopt(argc, argv, "n:h")) != -1) {
        switch (opt) {
        case 'n':
            num_lines = atoi(optarg);
            printf("Num_lines: %d\n", num_lines);
            break;
        case 'h':
            printf("./my-head [-n num] filename\n");
            break;
        default:
            exit_with_error();
        }
    }

    if (optind != argc - 1) {
        exit_with_error();
    }
    char *filename = argv[argc - 1];
    printf("%s\n", filename);

    // Fill out the code for opening the file below
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("my-head: cannot open file\n");
        exit(1);
    }


    
    // Fill out the code for printing out the first num_lines lines
    char buffer[BUFFER_SIZE];
    while (fgets(buffer, BUFFER_SIZE, fp) != NULL && num_lines > 0) {
        printf("%s", buffer);
        num_lines --;
    }
    return 0;
}

