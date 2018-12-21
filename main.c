#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<readline/readline.h>
#include<readline/history.h>

#define clear() printf("\033[H\033[J")

const int MAXCOM = 1000;
const int MAXLIST = 100;

char USER[BUFSIZ] = "N/A";

// Shell startup
void init_shell() {
    clear();
    printf("██╗     ███████╗███████╗██╗  ██╗                                                       \n");
    printf("██║     ██╔════╝██╔════╝██║  ██║                                                       \n");
    printf("██║     ███████╗███████╗███████║                                                       \n");
    printf("██║     ╚════██║╚════██║██╔══██║                                                       \n");
    printf("███████╗███████║███████║██║  ██║                                                       \n");
    printf("╚══════╝╚══════╝╚══════╝╚═╝  ╚═╝                                                       \n\n");
    printf("██╗   ██╗███████╗███████╗     █████╗ ████████╗    ██╗   ██╗ ██████╗ ██╗   ██╗██████╗   \n");
    printf("██║   ██║██╔════╝██╔════╝    ██╔══██╗╚══██╔══╝    ╚██╗ ██╔╝██╔═══██╗██║   ██║██╔══██╗  \n");
    printf("██║   ██║███████╗█████╗      ███████║   ██║        ╚████╔╝ ██║   ██║██║   ██║██████╔╝  \n");
    printf("██║   ██║╚════██║██╔══╝      ██╔══██║   ██║         ╚██╔╝  ██║   ██║██║   ██║██╔══██╗  \n");
    printf("╚██████╔╝███████║███████╗    ██║  ██║   ██║          ██║   ╚██████╔╝╚██████╔╝██║  ██║  \n");
    printf(" ╚═════╝ ╚══════╝╚══════╝    ╚═╝  ╚═╝   ╚═╝          ╚═╝    ╚═════╝  ╚═════╝ ╚═╝  ╚═╝  \n\n");
    printf("                         ██████╗ ██╗    ██╗███╗   ██╗    ██████╗ ██╗███████╗██╗  ██╗██╗\n");
    printf("                        ██╔═══██╗██║    ██║████╗  ██║    ██╔══██╗██║██╔════╝██║ ██╔╝██║\n");
    printf("                        ██║   ██║██║ █╗ ██║██╔██╗ ██║    ██████╔╝██║███████╗█████╔╝ ██║\n");
    printf("                        ██║   ██║██║███╗██║██║╚██╗██║    ██╔══██╗██║╚════██║██╔═██╗ ╚═╝\n");
    printf("                        ╚██████╔╝╚███╔███╔╝██║ ╚████║    ██║  ██║██║███████║██║  ██╗██╗\n");
    printf("                         ╚═════╝  ╚══╝╚══╝ ╚═╝  ╚═══╝    ╚═╝  ╚═╝╚═╝╚══════╝╚═╝  ╚═╝╚═╝\n\n");

    strcpy(USER, getenv("USER"));
    printf("USER: %s\n", USER);
    sleep(2);
    clear();
}

// Function to print Username, Hostname, Current Directory
void print_dir() {
    char hostname[BUFSIZ], cwd[BUFSIZ];
    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));
    printf("\n%s@%s %s", USER, hostname, cwd);
}

// Function to read input
int read_input(char *input) {
    char *buf;
    buf = readline("\n>>> ");

    if (strlen(buf)) { // strlen(buf) != 0
        add_history(buf);
        strcpy(input, buf);
        return 0;
    } else {
        return 1;
    }
}

// Function which the system command is executed
void exec_args(char **parsed_args) {
    // Forking a child
    pid_t pid = fork();

    if (pid == -1) {
        printf("\nError: Can't fork process");
        return;
    } else if (pid == 0) {
        if (execvp(parsed_args[0], parsed_args) < 0) {
            printf("\nError: Can't execute command");
        }
        exit(0);
    } else {
        // waiting for child to terminate
        wait(NULL);
        return;
    }
}

// Function which pipe system commands executed
void exec_args_piped(char **parsed_args, char **parsed_pipe) {
    // 0 mean end of read, 1 mean end of write
    int pipefd[2];
    pid_t p1, p2;

    if (pipe(pipefd) < 0) {
        printf("\nFailed to initialized pipe");
        return;
    }
    p1 = fork();
    if (p1 < 0) {
        printf("\nCan't fork");
        return;
    }

    if (p1 == 0) {
        // Child 1 executing..
        // It only needs to write at the write end
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        if (execvp(parsed_args[0], parsed_args) < 0) {
            printf("\nCan't execute command 1..");
            exit(0);
        }
    } else {
        // Parent executing
        p2 = fork();

        if (p2 < 0) {
            printf("\nCan't fork");
            return;
        }

        // Child 2 executing..
        // It only needs to read at the read end
        if (p2 == 0) {
            close(pipefd[1]);
            dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
            if (execvp(parsed_pipe[0], parsed_pipe) < 0) {
                printf("\nCan't execute command 2..");
                exit(0);
            }
        } else {
            // parent executing, waiting for two children
            wait(NULL);
            wait(NULL);
        }
    }
}

// Built-in help command
void help() {
    puts("\n-Use the shell at your own risk..."
         "\nList of Commands supported:"
         "\n>cd"
         "\n>ls"
         "\n>exit"
         "\n>all other general commands available in UNIX shell"
         "\n>pipe handling (but only two process)"
         "\n>improper space handling"
    );
}

// Function to execute builtin commands
int own_command_handler(char **parsed_args) {
    const int own_commands_num = 4;
    int i, switch_args = 0;
    char *own_commands[own_commands_num];
    char *username;

    own_commands[0] = "exit";
    own_commands[1] = "cd";
    own_commands[2] = "help";
    own_commands[3] = "hello";

    for (i = 0; i < own_commands_num; i++) {
        if (!strcmp(parsed_args[0], own_commands[i])) { // strcmp(parsed_args[0], own_commands[i]) == 0
            switch_args = i + 1;
            break;
        }
    }

    switch (switch_args) {
        case 1:
            printf("\nYeah, you'd better use other shell.\n");
            exit(0);
        case 2:
            chdir(parsed_args[1]);
            return 1;
        case 3:
            help();
            return 1;
        case 4:
            printf("\nHello %s.\nThis is shell called lssh "
                   "made for studying basic of shell."
                   "\nUse help to know more..\n",
                   USER);
            return 1;
        default:
            break;
    }

    return 0;
}

// Function for find pipe
int parse_pipe(char *str, char **piped_str) {
    int i;
    for (i = 0; i < 2; i++) {
        piped_str[i] = strsep(&str, "|");
        if (piped_str[i] == NULL) {
            break;
        }
    }

    if (piped_str[1] == NULL) {
        return 0; // returns zero if no pipe is found.
    } else {
        return 1;
    }
}

// Function for parsing command words
void parse_space(char *str, char **parsed) {
    int i;
    for (i = 0; i < MAXLIST; i++) {
        parsed[i] = strsep(&str, " ");

        if (parsed[i] == NULL) {
            break;
        }
        if (strlen(parsed[i]) == 0) {
            i--;
        }
    }
}

int process_string(char *str, char **parsed, char **parsed_pipe) {
    char *str_piped[2];
    int piped = 0;

    piped = parse_pipe(str, str_piped);

    if (piped) {
        parse_space(str_piped[0], parsed);
        parse_space(str_piped[1], parsed_pipe);
    } else {
        parse_space(str, parsed);
    }

    if (own_command_handler(parsed)) {
        return 0;
    } else {
        return 1 + piped;
    }
}

int main(int argc, const char *argv[]) {
    char input_string[MAXCOM], *parsed_args[MAXLIST];
    char *parsed_args_piped[MAXLIST];

    int exec_flag = 0;

    init_shell();

    while (1) {
        // print shell line
        print_dir();

        // take input
        if (read_input(input_string)) {
            continue;
        }

        // if "exit" is entered,
        if (!strcmp(input_string, "exit")) { // if input_string is "exit"
            break;
        }

        // process
        exec_flag = process_string(input_string, parsed_args, parsed_args_piped);
        // execflag returns zero if there is no command
        // or it is a builtin command,
        // 1 if it is a simple command
        // 2 if it is including a pipe.

        // execute
        if (exec_flag == 1)
            exec_args(parsed_args);

        if (exec_flag == 2)
            exec_args_piped(parsed_args, parsed_args_piped);
    }

    return 0;
}