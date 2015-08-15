#ifndef _H_PERF_EVENT
#define _H_PERF_EVENT

#include <linux/perf_event.h>
#include <sys/types.h>

struct perf_event_open {
        struct  perf_event_attr attr;
        pid_t                   pid;
        int                     cpu;
        int                     group_id;
        unsigned long           flags;
	char 			*name;
};

static long perf_event_open(struct perf_event_attr *,pid_t,int,int,unsigned long);

struct mmap_pages *perf_event_create(struct perf_event_open *);

#endif
