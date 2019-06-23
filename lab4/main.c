#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>
#include "elftool.h"

#define ILLEGAL printf("** This command is illegal now.\n")

#define BUFFER_LEN 256
char buffer[BUFFER_LEN];

#define INPUT_LEN 32
#define INPUT_SIZE 8
char input[INPUT_SIZE][INPUT_LEN];
int input_len;

enum {
    non_loaded,
    loaded,
    running
} status;

pid_t child;
char program[INPUT_LEN];
int pid_status;

void print_error(const char *s) {
    printf("** Error: %s\n", s);
    exit(-1);
}

void elf_parse(const char *file_name) {
    elf_handle_t *eh = NULL;
    elf_strtab_t *tab = NULL;

    elf_init();
    if(!(eh = elf_open(file_name)))
        print_error("unable to open the elf.");
    if(elf_load_all(eh) < 0) {
        elf_close(eh);
        print_error("unable to load the elf.");
    }
    for(tab = eh->strtab; tab; tab = tab->next)
        if(tab->id == eh->shstrndx)
            break;
    if(!tab) {
        elf_close(eh);
        print_error("section header string table not found.");
    }

    for(int i = 0; i < eh->shnum; i++) {
        if (!strcmp(&tab->data[eh->shdr[i].name], ".text"))
            printf("** program '%s' loaded. entry point 0x%llx, vaddr %llx, offset 0x%llx, size 0x%llx\n", file_name, eh->shdr[i].addr, eh->shdr[i].addr, eh->shdr[i].offset, eh->shdr[i].size);
    }
}

int run_single() {
    if(ptrace(PTRACE_SINGLESTEP, child, 0, 0) < 0)
        print_error("PTRACE_SINGLESTEP failed!");
    if (waitpid(child, &pid_status, 0) < 0)
        print_error("waitpid failed!");
    return 0;
}

int run_program() {
    if (ptrace(PTRACE_CONT, child, 0, 0) < 0)
        print_error("PTRACE_CONT failed!");
    printf("** program sample/hello64 is already running.\n");
    if (waitpid(child, &pid_status, 0) < 0)
        print_error("waitpid failed!");
    return 0;
}

int start_program(const char *file_name) {
    child = fork();
    if (child < 0)
        print_error("fork failed!");
    if (!child) {
        if (ptrace(PTRACE_TRACEME, 0, 0, 0) < 0)
            print_error("PTRACE_TRACEME failed!");
        if (execlp(file_name, file_name, NULL) < 0)
            print_error("execlp failed!");
        exit(0);
    }
    else {
        if (waitpid(child, &pid_status, 0) < 0)
            print_error("waitpid failed!");
        if (ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_EXITKILL) < 0 )
            print_error("PTRACE_SETOPTIONS failed!");
        printf("** pid %d\n", child);
        status = running;
    }
    return 0;
}

int load_program(const char *file_name) {
    if (access(file_name, 0)) {
        printf("** The executable does not exist.\n");
        return -1;
    }

    elf_parse(file_name);
    strcpy(program, file_name);
    status = loaded;

    return 0;
}

int command() {
    if (!strcmp(input[0], "load") && status == non_loaded)
        return load_program(input[1]);
    else if (!strcmp(input[0], "start") && status == loaded)
        return start_program(program);
    else if ((!strcmp(input[0], "r") || !strcmp(input[0], "run")) && status == running)
        return run_program();
    else if (!strcmp(input[0], "si") && status == running)
        return run_single();

    ILLEGAL;
    return -1;
}

int wait_input() {
    printf("sdb> ");
    fgets(buffer, BUFFER_LEN, stdin);
    buffer[strlen(buffer) - 1] = '\0';

    char *token;
    input_len = 0;
    token = strtok(buffer, " ");
    while (token) {
        strcpy(input[input_len], token);
        input_len++;
        token = strtok(NULL, " ");
    }

    return command();
}

int main(int argc, char **argv) {
    status = non_loaded;
    if (argc < 2)
        while (wait_input());
    else
        load_program(argv[1]);

    while (1) {
        if (wait_input())
            continue;
        if (!WIFSTOPPED(pid_status))
            break;
    }

    return 0;
}
