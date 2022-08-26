#ifndef __MAINLOOP_H__
#define __MAINLOOP_H__

#include <sys/epoll.h>

typedef void (*mainloop_event_func)(int fd, uint32_t events, void *user_data);
typedef void (*mainloop_destroy_func)(void *user_data);

void mainloop_init(void);
void mainloop_quit(void);
int mainloop_add_fd(int fd, uint32_t events, mainloop_event_func callback,
                    void *user_data, mainloop_destroy_func destroy);
int mainloop_run(void);
#endif