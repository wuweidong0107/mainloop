#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>

#include "mainloop.h"

static void cmd_help();

static void cmd_exit()
{
    mainloop_quit();
}

typedef void (*command_func_t)();
static struct {
    char *cmd;
    command_func_t func;
    char *doc;
} command[] = {
    {"help", cmd_help, "\tDisplay help message"},
    {"exit", cmd_exit, "\tExit"},
    {}
};

static void cmd_help()
{
    int i;

    printf("Commands:\n");
    for(i=0; command[i].cmd; i++)
        printf("\t%-15s\t%s\n", command[i].cmd, command[i].doc);
}

static void print_prompt(void)
{
    printf("[mainloop]$ ");
    fflush(stdout);
}

static void prompt_read_cb(int fd, uint32_t events, void *user_data)
{
    ssize_t read;
    size_t len = 0;
    char *line = NULL;
    int i;

	if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
		mainloop_quit();
		return;
	}

    read = getline(&line, &len, stdin);
    if (read < 0) {
        free(line);
        return;
    }

    if (read <= 1) {
        cmd_help();
    }
    line[read-1] = '\0';

    for(i=0; command[i].cmd; i++)
        if(!strcmp(command[i].cmd, line))
            break;
    if(command[i].cmd)
        command[i].func();

    print_prompt();
    free(line);
}

int main(void)
{
    int fd;
    int ret;

    mainloop_init();

	if (mainloop_add_fd(fileno(stdin),
				EPOLLIN | EPOLLRDHUP | EPOLLHUP | EPOLLERR,
				prompt_read_cb, NULL, NULL) < 0) {
		fprintf(stderr, "Failed to initialize console\n");

		return EXIT_FAILURE;
	}

    print_prompt();
    ret = mainloop_run();
    return 0;
}