#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "mainloop.h"

static void prompt_read_cb(int fd, uint32_t events, void *user_data)
{
	if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
		mainloop_quit();
		return;
	}
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

    ret = mainloop_run();
    return 0;
}