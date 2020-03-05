//
// Created by Benxin Niu on 2020-03-05.
//

#include "bento_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <sys/stat.h>
#include <err.h>

void run_bento_shell(void) {
    int status = 1;
    do {
        printf("bento_shell >>");
        char *input = read_input(100);
        int const param_len = get_number_of_param(input);
        char *param_list[param_len];
        if (param_len != 0) {
            char *param = strtok(input, " ");
            int idx = 0;
            while (idx <= param_len) {
                param_list[idx] = param;
                idx++;
                param = strtok(NULL, " ");
            }
            char *command = get_command_location(param_list[0]);
            if (command == NULL) {
                printf("%s Command not found by bento shell...\n", input);
            } else {
                param_list[0] = command;
                run_command(command, param_list);
            }
        }
    } while (status);
}

int get_number_of_param(char *input) {
    int num = 0;
    char *input_cpy = malloc(strlen(input));
    strcpy(input_cpy, input);
    char *command = strtok(input_cpy, " ");
    while (command != NULL) {
        num++;
        command = strtok(NULL, " ");
    }
    free(input_cpy);
    return num;
}

void run_command(const char *path, char *const *parmList) {
    pid_t child = fork();
//    printf("The child PID is %d\n", child);
    switch (child) {
        case -1:
            err(-1, "Error in fork()");

        case 0: {
//            printf("PID 0: I must be the child!\n");
            execv(path, parmList);
            err(-1, "Failed to execute binary");
            break;
        }

        default: {
//            printf("PID %d: I must be the parent!\n", child);

            int status;
            if (waitpid(child, &status, 0) == -1) {
                err(-1, "Failed to waitpid()");
            }
            if (WIFEXITED(status)) {
//                printf("child exited with code: %d\n", WEXITSTATUS(status));
            }
            break;
        }
    }
}

char *read_input(int max_input_len) {
    int idx = 0;
    char *input = malloc(sizeof(char) * max_input_len);

    while (true) {
        int c = getchar();
        if (c == EOF || c == '\n') {
            input[idx] = '\0';
            return input;
        } else {
            input[idx] = c;
        }
        idx++;
        if (idx >= max_input_len) {
            printf("Max input size (%d) reached", max_input_len);
            return input;
        }
    }
}

char *get_command_location(char *command) {
    char *command_location = NULL;
    char *env_path = getenv("PATH");
    char *path = malloc(strlen(env_path));
    strcpy(path, env_path);
    if (path == NULL) {
        err(1, "shit error loading ENV VAR $PATH");
    }
    char *selected_bin = strtok(path, ":");
    while (selected_bin != NULL) {
        char *command_path = bento_concat_str(selected_bin, "/");
        command_path = bento_concat_str(command_path, command);
        if (file_exist(command_path)) {
            command_location = command_path;
            break;
        }
        selected_bin = strtok(NULL, ":");
    }
    free(path);
    return command_location;
}

bool file_exist(char *filename) {
    struct stat buffer;
    return (stat(filename, &buffer) == 0);
}

char *bento_concat_str(char *str1, char *str2) {
    char *dest = (char *) malloc(1 + strlen(str1) + strlen(str2));
    strcpy(dest, str1);
    strcat(dest, str2);
    return dest;
}