# mainloop
A simple event loop come from bluez.

Backend is epoll.

# Usage
see example.c

```
$ make
$ ./example 
[mainloop]$ ls
[mainloop]$ ifconfig
[mainloop]$ help
Commands:
	help           		Display help message
	exit           		Exit
[mainloop]$ exit
```


# API
```C
void mainloop_init(void);

void mainloop_quit(void);

int mainloop_add_fd(int fd, uint32_t events, mainloop_event_func callback,
                    void *user_data, mainloop_destroy_func destroy);

int mainloop_run(void);
```
