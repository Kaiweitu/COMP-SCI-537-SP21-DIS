/**
 * Simple my-head implementation
 *
 * Usage: ./my-head [-n num] filename
 *
 * Copyright Kaiwei Tu
 */
#include <stdio.h>
#include <unistd.h>
#include <getopt.h>
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

    // Fill out the code for opening the file below
    FILE *fp = NULL;
    

    // Fill out the code for printing out the first num_lines lines
    char buffer[BUFFER_SIZE];

    return 0;
}

