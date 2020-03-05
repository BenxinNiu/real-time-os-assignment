#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <sys/stat.h>
#include <err.h>
#include "bento_shell.h"

int main() {

    char *sep = "/";
    char * str1 = "bin";
    char * str2 = "ls";

    run_bento_shell();
    return 0;
}