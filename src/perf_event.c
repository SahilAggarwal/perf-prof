#include <stdlib.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "perf_event.h"
#include "mmap_page.h"

static long
perf_event_open(struct perf_event_attr *event,pid_t pid,
                int cpu,int group_id,unsigned long flags)
{
        return syscall(__NR_perf_event_open,event,pid,cpu,group_id,flags);
}

struct mmap_pages *
perf_event_create(struct perf_event_open *e_open)
{
	int fd = -1;

	e_open->attr.disabled 		= 1;
	e_open->attr.size 		= sizeof(e_open->attr);
	e_open->attr.type 		= PERF_TYPE_TRACEPOINT;
	e_open->attr.config		= 312;
	e_open->attr.sample_period	= 1;
	e_open->attr.sample_type 	= PERF_SAMPLE_TIME 	|
					  PERF_SAMPLE_RAW  	|
					  PERF_SAMPLE_TID  	|
					  PERF_SAMPLE_STREAM_ID	;
	
	fd = perf_event_open(&e_open->attr,e_open->pid,e_open->cpu	\
				,e_open->group_id,e_open->flags);

	if(fd < 0) {
		fprintf(stderr,"Failed to open event FD\n");
		return NULL;
	}
	struct mmap_pages *mmap_pages = NULL;
	mmap_pages = mmap_pages__new(fd,1);
	if(mmap_pages != NULL) {
		mmap_pages->attr = e_open->attr;
		fcntl(fd, F_SETFL, O_NONBLOCK);
	}
	return mmap_pages;
}
