#ifndef _H_MMAP_PAGE
#define _H_MMAP_PAGE

#include <linux/perf_event.h>

struct mmap_pages {
	int 			n;
	int 			fd;
	struct perf_event_attr  attr;
	__u64			mask;
	void 			*base;
	void 			*wrap_base;
	__u64			read;
};

struct mmap_pages *mmap_pages__new(int, int);

#endif

