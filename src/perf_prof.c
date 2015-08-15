#include <stdlib.h>
#include <stdio.h>
#include <asm/unistd.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "cpu_map.h"
#include "perf_prof.h"
#include "mmap_page.h"
#include "output.h"

__u64 		page_size;
int		cpu = -1;
int		pid = -1;

struct wakeup wakeup = {
	.ctrlc = 0,
	.rd_fd = -1,
	.wr_fd = -1
};

void signal_handler() {
	char data[1] = { 1 };
	__sync_add_and_fetch(&wakeup.ctrlc,1);
	write(wakeup.wr_fd, data, 1);
}

static struct mmap_pages *
create_event(struct perf_event_thread *e_thread) 
{
	struct mmap_pages *mmap_pages = NULL;
	mmap_pages = perf_event_create(&e_thread->e_open);
	if(mmap_pages == NULL) {
		fprintf(stderr,"Failed to create mmap_page\n");
		return NULL;
	}
	return mmap_pages;
}

int create_event_thread(struct perf_event_thread *e_thread)
{
	if(pthread_create(&e_thread->thread,NULL,perf_event_loop, e_thread)) {
                fprintf(stderr,"Failed to start pthread\n");
                return -1;
        }
	return 0;
}

void *perf_event_loop(void *arg)
{
        struct perf_event_thread *e_thread      = (struct perf_event_thread *)arg;
        struct mmap_pages 	 *mmap_pages    = e_thread->mmap_pages;
        struct pollfd 		 pollfd[]       = { {mmap_pages->fd, POLLIN, 0} ,  	 \
                                                    {wakeup.rd_fd, POLLIN | POLLHUP , 0} \
                                                  };

	struct output_records	 out_records    = { {write_output},
						    e_thread->e_open.attr,
						    (FILE *)stdout  
						  };

        while(!__sync_fetch_and_or(&wakeup.ctrlc,0)) {
                int ret = poll(pollfd, 2, -1);
                if(ret == -1) {
                        fprintf(stderr,"Failed to start poll");
                        continue;
                }
                mmap_pages_read(mmap_pages,&out_records.reader);
        }
}

int main(int argc, char **argv)
{
	page_size = sysconf(_SC_PAGE_SIZE);
	int pipe_fd[2];
	int nthreads = 0;

	//struct perf_event_thread 	e_threads[MAX_NR_CPUS];
	struct perf_event_thread	*e_threads[MAX_NR_CPUS];
	struct cpu_map 			*cpus = NULL;

	struct perf_event_open deflt = {
		.cpu            = -1,
		.pid            = -1,
		.group_id       = -1,
		.flags          = 0
	};

	if(pipe2(pipe_fd, O_NONBLOCK) == -1) {
		fprintf(stderr,"Failed to create wakeup pipe.");
		exit(1);
	}
	wakeup.rd_fd = pipe_fd[0];
	wakeup.wr_fd = pipe_fd[1];
	signal(SIGINT, signal_handler);
	signal(SIGCHLD, signal_handler);

	struct perf_event_open 	e_open = deflt;
	struct mmap_pages *mmap_pages = NULL;
	if(e_open.cpu == -1 && e_open.pid == -1) {
		struct perf_event_thread *e_thread      = (struct perf_event_thread *)
							     malloc(sizeof(*e_thread));
		cpus = cpu_map__read_all();
	
		if(cpus == NULL) {
			fprintf(stderr,"Failed to map CPUs");
			exit(1);
		}

		int i=0;
		for(; i< cpus->nr; i++) {
			e_open.cpu 		= cpus->map[i];
			e_thread->e_open 	= e_open;
			mmap_pages		= create_event(e_thread);
			if(mmap_pages != NULL) {
				e_thread->mmap_pages    = mmap_pages;
				e_threads[nthreads] 	= e_thread;			
				if(!create_event_thread(e_threads[nthreads]))
					nthreads++;
			}
		}
	} else {
		struct perf_event_thread *e_thread      = (struct perf_event_thread *)
                                                     malloc(sizeof(*e_thread));
		e_open.cpu = cpu;
		e_thread->e_open = e_open;
		mmap_pages = create_event(e_thread);
		if(mmap_pages != NULL) {
			e_thread->mmap_pages = mmap_pages;
			e_threads[nthreads] = e_thread;
			if(!create_event_thread(e_threads[nthreads])) 
                                        nthreads++;
		}
	}
	free(cpus);	
	int i;
	for(i=0; i < nthreads; i++) 
	{
		if(syscall(__NR_ioctl, e_threads[i]->mmap_pages->fd, PERF_EVENT_IOC_ENABLE, 0)) {
			fprintf(stderr,"Failed to enable sampling\n");
			continue;
		}
	}

	for(i=0; i < nthreads; i++)
		pthread_join(e_threads[i]->thread,NULL);
}

