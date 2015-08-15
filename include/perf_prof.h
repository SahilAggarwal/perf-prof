#ifndef _H_PERF_THREAD
#define _H_PERF_THREAD

#include "perf_event.h"

struct perf_event_thread {
        struct perf_event_open  e_open;
	struct mmap_pages	*mmap_pages;
	pthread_t		thread;
};

struct wakeup {
	int ctrlc;
	int wr_fd;
	int rd_fd;
};

static struct mmap_pages *create_event(struct perf_event_thread *);

int create_event_thread(struct perf_event_thread *);

void *perf_event_loop(void *);

#endif



