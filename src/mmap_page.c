#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include "mmap_page.h"

#include "kernel_func.h"
#include "output.h"

#define two_to_the(n) (1 << (n))

// From src/perf_prof.c
extern int page_size;

struct mmap_pages *mmap_pages__new(int fd,int n)
{
	int pages = two_to_the(n);
	int mmap_len = (pages + 2) * page_size;
	struct mmap_pages *mmap_pages = calloc(1,sizeof(*mmap_pages));
	mmap_pages->n 		= n;
	mmap_pages->fd 		= fd;
	mmap_pages->mask 	= pages * page_size - 1;

	mmap_pages->wrap_base	= mmap(NULL, mmap_len,PROT_READ | PROT_WRITE, \
					MAP_PRIVATE | MAP_ANONYMOUS, fd, 0);

	mmap_pages->base 	= mmap(mmap_pages->wrap_base + page_size,     \
					mmap_len - page_size, PROT_READ |     \
					PROT_WRITE, MAP_SHARED | MAP_FIXED,   \
					fd, 0);

	return mmap_pages->base == MAP_FAILED ? NULL : mmap_pages;
} 

static inline __u64 
mmap_pages__read_head(struct perf_event_mmap_page *meta)
{
	__u64 head = ACCESS_ONCE(meta->data_head);
	rmb();
	return head;
}

static inline void 
mmap_pages__write_tail(struct perf_event_mmap_page *meta, __u64 tail)
{
	mb();
	meta->data_tail = tail;
}

int mmap_pages_read(struct mmap_pages *mp,struct buf_reader *reader)
{
	__u64 head = mmap_pages__read_head(mp->base);
	unsigned char *data = (unsigned char *)mp->base + page_size;
	int size = 0;
	int nread = 0;
	int ret;
	int nleft = 0;

	void *buf = NULL;


	printf("Got something\n");
	if(mp->read == head)
		return 0;
	
	size = head - mp->read;
	if((mp->read & mp->mask) + size != (head & mp->mask) && 
		((mp->read + size) &  mp->mask) != 0 ) {

		buf = &data[mp->read & mp->mask];
		size = mp->mask + 1 - (mp->read & mp->mask);
		nread = reader->read(buf,size,reader);
		if(nread < 0) {
			ret = -1;
			goto out;
		}
		mp->read += nread;
		nleft = size - nread;
		if((__u64)nleft >= page_size) {
			fprintf(stderr,"Record too big, doesnt make sense\n");	
			ret = -1;
			goto out;
		}
	}
	memcpy(data - nleft,data + (mp->read & mp->mask), nleft);
	buf = &data[(mp->read + nleft) & mp->mask] - nleft;
	size = head - mp->read;
	nread = reader->read(buf,size,reader);
	mp->read += nread;
	ret = nread;
	
	out:
		return ret;
}
