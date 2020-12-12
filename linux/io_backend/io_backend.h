#ifndef IO_BACKEND_H_
#define IO_BACKEND_H_


typedef struct (*io_event_func)(int fd, void *arg);

typedef struct {
	int fd;
} ioev_backend_t;


int ioev_backend_create(io_event_backend_t *handle);
int ioev_backend_set(io_event_backend_t *handle, int fd, int ev_diretion, io_event_func func);
int ioev_backend_add(io_event_backend_t *handle, int fds[], int nfd);
int ioev_backend_wait(io_event_backend_t *handle);
int ioev_backend_destroy(io_event_backend_t *handle);

#endif