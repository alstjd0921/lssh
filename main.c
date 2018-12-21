#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

void clear() {
    printf("\033[H\033[J");
}

void init_shell() {
    clear();

}

int main(int argc, const char *argv[]) {
    init_shell();

    return 0;
}