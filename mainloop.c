#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

#include "mainloop.h"

#define MAX_MAINLOOP_ENTRIES (128)
#define MAX_EPOLL_EVENTS     (10)

struct mainloop_data {
    int fd;
    uint32_t events;
    mainloop_event_func callback;
    mainloop_destroy_func destroy;
    void *userdata;
};

static int epoll_fd;
static int epoll_terminate;
static int exit_status = EXIT_SUCCESS;
static struct mainloop_data *mainloop_list[MAX_MAINLOOP_ENTRIES];

void mainloop_init(void)
{
    unsigned int i;

    epoll_fd = epoll_create1(EPOLL_CLOEXEC);
    
    for(i=0; i<MAX_MAINLOOP_ENTRIES; i++) {
        mainloop_list[i] = NULL;
    }

    epoll_terminate = 0;
}

void mainloop_quit(void)
{
    epoll_terminate = 1;
}

int mainloop_add_fd(int fd, uint32_t events, mainloop_event_func callback,
                    void *user_data, mainloop_destroy_func destroy)
{
    struct mainloop_data *data;
    struct epoll_event ev;
    int err;
    
    if (fd<0 || fd>(MAX_MAINLOOP_ENTRIES -1) || !callback)
        return -EINVAL;
    
    data = malloc(sizeof(*data));
    if (!data)
        return -ENOMEM;
    
    memset(data, 0, sizeof(*data));
    data->fd = fd;
    data->events = events;
    data->callback = callback;
    data->userdata = user_data;
    data->destroy = destroy;
    
    memset(&ev, 0, sizeof(ev));
    ev.events = events;
    ev.data.ptr = data;

    err = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, data->fd, &ev);
    if (err<0) {
        free(data);
        return err;
    }
    mainloop_list[fd] = data;

    return 0;
}

int mainloop_run(void)
{
    int i;

    while (!epoll_terminate) {
        struct epoll_event events[MAX_EPOLL_EVENTS];
        int n, nfds;

        nfds = epoll_wait(epoll_fd, events, MAX_EPOLL_EVENTS, -1);
        if (nfds < 0)
            continue;
 
        for(n=0; n<nfds; n++) {
            struct mainloop_data *data = events[n].data.ptr;
            data->callback(data->fd, events[n].events, data->userdata);
        }
    }

    for (i=0; i<MAX_MAINLOOP_ENTRIES; i++) {
        struct mainloop_data *data = mainloop_list[i];
        mainloop_list[i] = NULL;

        if (data) {
            epoll_ctl(epoll_fd, EPOLL_CTL_DEL, data->fd, NULL);

            if (data->destroy)
                data->destroy(data->userdata);
            free(data);
        }
    }

    close(epoll_fd);
    epoll_fd = 0;
    return exit_status;
}