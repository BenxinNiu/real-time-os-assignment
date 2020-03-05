//
// Created by Benxin Niu on 2020-03-05.
//

#include <stdbool.h>

#ifndef ASSIGN3_BENTO_SHELL_H
#define ASSIGN3_BENTO_SHELL_H

#endif //ASSIGN3_BENTO_SHELL_H

void run_bento_shell(void);

bool file_exist(char *filename);

char *get_command_location(char *command);

char *bento_concat_str(char *str1, char *str2);

char *read_input(int max_input_len);

void run_command(const char *path, char **parmList);

int get_number_of_param(char *input);

void try_redirect(char **param_list);

char *update_history(char *const input, char *history);

void print_history(char *const history);