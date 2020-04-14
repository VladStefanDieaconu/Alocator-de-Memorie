// Copyright 2017 Vlad-Stefan DIEACONU, 311CA
/*
    Project CA - allocator - skel

    Darius Neatu  <darius.neatu@cti.pub.ro>
    Rares Cheseli <rares.cheseli@stud.acs.upb.ro>
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./arena.h"

#define MAX_LINE_LEN 1000
#define MAX_CMD_ARGS 10




int cmd_read(int *args_count, char *args[MAX_CMD_ARGS]);
void cmd_execute(Arena *arena, char *args[MAX_CMD_ARGS]);
void cmd_free(int args_count, char *args[MAX_CMD_ARGS]);

int main() {
    // cmd: cmd_name argument1 argument2 ...
    // args = {cmd_name, argument1, argument2, ... }
    // args_count = length of args
    int args_count;
    char *args[MAX_CMD_ARGS];
    Arena arena;

    while (1) {
        // read new command from stdin
        if (!cmd_read(&args_count, args)) {
            break;  // no command was found, break loop
        }

        cmd_execute(&arena, args);  // execute command
        cmd_free(args_count, args);  // free memory allocated for args
    }

    return 0;
}

// read a new command from stdin
int cmd_read(int *args_count, char *args[MAX_CMD_ARGS]) {
    char line[MAX_LINE_LEN + 1];  //  buffer for a line

    // try to read a new command (a new line)
    if (!fgets(line, MAX_LINE_LEN, stdin)) {
        return 0;  // fail
    }

    // delete extra '\n'
    int len = strlen(line);
    if (line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }

    // split string in tokens ("words")
    const char *delimiters = " ";       // delimiters
    int count = 0;                      // number of tokens
    char *token = strtok(line, delimiters);  // get first token

    while (token != NULL) {
        // save current token
        len = strlen(token) + 1;  // +1 because I need to store '\0'
        args[count] = (char *) malloc(len * sizeof(char));
        snprintf(args[count], 100 * sizeof(char), "%s", token);

        // get next token
        token = strtok(NULL, delimiters);
        ++count;
    }

    *args_count = count;  // save arguments count

    return 1;            // success
}


// free memory allocated for args
void cmd_free(int args_count, char *args[MAX_CMD_ARGS]) {
    for (int i = 0; i < args_count; ++i) {
        free(args[i]);
    }
}

// ONLY FOR DEBUG
void print_args(int args_count, char *args[MAX_CMD_ARGS]) {
    printf("cmd = (%s)\n", args[0]);
    for (int i = 0; i < args_count; ++i) {
        printf("arg[%d] = (%s)\n", i, args[i]);
    }
    printf("\n");
}

// execute a command
void cmd_execute(Arena *arena, char *args[MAX_CMD_ARGS]) {
    char *cmd = args[0];

    if (!strcmp(cmd, "INITIALIZE")) {
        // implement INITIALIZE
        INITIALIZE(arena, atoi(args[1]));

    } else if (!strcmp(cmd, "FINALIZE")) {
        // implement FINALIZE
        FINALIZE(arena);

    } else if (!strcmp(cmd, "DUMP")) {
        // implement DUMP
        DUMP(arena);

    } else if (!strcmp(cmd, "ALLOC")) {
        // implement ALLOC
        ALLOC(arena, atoi(args[1]));

    } else if (!strcmp(cmd, "FREE")) {
        // implement FREE
        FREE(arena, atoi(args[1]));

    } else if (!strcmp(cmd, "FILL")) {
        // implement FILL
        FILL(arena, atoi(args[1]), atoi(args[2]), atoi(args[3]));

    } else if (!strcmp(cmd, "SHOW")) {
        // implement SHOW
        SHOW(arena, args[1]);

    } else if (!strcmp(cmd, "ALLOCALIGNED")) {
        // implement ALLOCALIGNED
    } else if (!strcmp(cmd, "REALLOC")) {
        // implement REALLOC
    } else if (!strcmp(cmd, "SHOW MAP")) {
        // implement SHOW MAP
    } else {
        fprintf(stderr, "[ERROR] unknown command (%s)\n", cmd);
    }
}

