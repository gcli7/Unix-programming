#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/user.h>
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

int get_all_registers() {
    struct user_regs_struct regs;
    if(ptrace(PTRACE_GETREGS, child, 0, &regs) < 0)
        print_error("PTRACE_GETREGS failed!");
    printf("RAX %llx\tRBX %llx\tRCX %llx\tRDX %llx\n", regs.rax, regs.rbx, regs.rcx, regs.rdx);
    printf("R8  %llx\tR9  %llx\tR10 %llx\tR11 %llx\n", regs.r8, regs.r9, regs.r10, regs.r11);
    printf("R12 %llx\tR13 %llx\tR14 %llx\tR15 %llx\n", regs.r12, regs.r13, regs.r14, regs.r15);
    printf("RDI %llx\tRSI %llx\tRBP %llx\tRSP %llx\n", regs.rdi, regs.rsi, regs.rbp, regs.rsp);
    printf("RIP %llx\tFLAGS %016llx\n", regs.rip, regs.eflags);
    return 0;
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

int show_help() {
	printf("** - break {instruction-address}: add a break point\n");
	printf("** - cont: continue execution\n");
	printf("** - delete {break-point-id}: remove a break point\n");
	printf("** - disasm addr: disassemble instructions in a file or a memory region\n");
	printf("** - dump addr [length]: dump memory content\n");
	printf("** - exit: terminate the debugger\n");
	printf("** - get reg: get a single value from a register\n");
	printf("** - getregs: show registers\n");
	printf("** - help: show this message\n");
	printf("** - list: list break points\n");
	printf("** - load {path/to/a/program}: load a program\n");
	printf("** - run: run the program\n");
	printf("** - vmmap: show memory layout\n");
	printf("** - set reg val: get a single value to a register\n");
	printf("** - si: step into instruction\n");
	printf("** - start: start the program and stop at the first instruction\n");
    return -1;
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
    else if (!strcmp(input[0], "getregs") && status == running)
        return get_all_registers();
    else if (!strcmp(input[0], "help"))
        return show_help();

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
