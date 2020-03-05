//
// Created by Benxin Niu on 2020-03-05.
//

#include "bento_shell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <err.h>

void run_bento_shell(void) {
    int status = 1;
    int counter = 0;
    char const *hisotry_cmd = "history";
    char *history = "";
    do {
        printf("bento_shell >>");
        char *input = read_input(100);
        int const param_len = get_number_of_param(input);
        char *param_list[param_len];
        if (param_len > 0) {
            counter++;
            history = update_history(input, history);
            char *param = strtok(input, " ");
            int idx = 0;
            while (idx <= param_len) {
                param_list[idx] = param;
                idx++;
                param = strtok(NULL, " ");
            }
            if (strcmp(param_list[0], hisotry_cmd) == 0) {
                print_history(history);
            } else {
                char *command_location = get_command_location(param_list[0]);
                if (command_location == NULL) {
                    printf("%s Command not found by bento shell...\n", input);
                } else {
                    param_list[0] = command_location;
                    run_command(command_location, param_list);
                }
            }
        }
    } while (status);
}

char *update_history(char *const input, char *history) {
    history = bento_concat_str(history, "\t");
    return bento_concat_str(history, input);
}

void print_history(char *const history) {
    char *history_cpy = malloc(strlen(history));
    strcpy(history_cpy, history);
    char *cmd = strtok(history_cpy, "\t");
    while (cmd != NULL) {
        printf("%s\n", cmd);
        cmd = strtok(NULL, "\t");
    }
}

void run_command(const char *path, char **param_list) {
    pid_t child = fork();
    switch (child) {
        case -1:
            err(-1, "Error in fork()");

        case 0: {
            try_redirect(param_list);
            execv(path, param_list);
            printf("debug: %s", param_list[1]);
            err(-1, "Failed to execute binary");
        }

        default: {
            int status;
            if (waitpid(child, &status, 0) == -1) {
                err(-1, "Failed to waitpid()");
            }
            if (WIFEXITED(status)) {
                printf("exited with code: %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("terminated by signal: %d\n", WTERMSIG(status));
            } else if (WIFSTOPPED(status)) {
                printf("stopped by signal: %d\n", WSTOPSIG(status));
            }
            break;
        }
    }
}

void try_redirect(char **param_list) {
    char *const std_out = ">";
    char *const std_err = "2>";
    int idx = 0;
    int fd = 0;
    char *param = param_list[idx];
    while (param != NULL) {
        if (strcmp(param, std_out) == 0) {
            fd = 1;
            break;
        } else if (strcmp(param, std_err) == 0) {
            fd = 2;
            break;
        }
        param = param_list[++idx];
    }
    if (fd != 0) {
        if (param_list[idx + 1] != NULL) {
            int redirect = open(param_list[idx + 1], O_CREAT | O_RDWR, 0666);
            close(fd); /* Close original stdout */
            dup2(redirect, fd);
            param_list[idx] = NULL;
        } else {
            printf("no output file specified, skipping redirect\n");
        }
    }
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

char *bento_concat_str(char *const str1, char *const str2) {
    char *dest = (char *) malloc(1 + strlen(str1) + strlen(str2));
    strcpy(dest, str1);
    strcat(dest, str2);
    return dest;
}