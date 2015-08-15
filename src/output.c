#include <stdio.h>
#include <stdlib.h>

#include "output.h"

__u64 write_output(void *buf, __u64 size, struct output_records *out)
{
	int nread = 0;
	struct perf_event_attr attr = out->attr;
	printf("Size:%d\n",sizeof(struct sched_switch));
	while(nread < size) {

		struct perf_event_header *header = buf;
		void *sample = (void *)(header + 1);
		
		if(nread + header->size > size) 
			break;
		
		switch(header->type) {

			case PERF_RECORD_SAMPLE:

				if(attr.sample_type & PERF_SAMPLE_TID) {
                                        struct perf_record_sample_tid *tid = sample;
                                        fprintf(out->file,"TID: %d, PID: %d\n",    \
                                                           tid->tid,   		   \
                                                           tid->pid    		   );
					sample += sizeof(*tid);
                                }

                                if(attr.sample_type & PERF_SAMPLE_TIME) {
                                        struct perf_record_sample_time *time = sample;
                                        fprintf(out->file,"TIME: %ld\n",time->time);
					sample += sizeof(*time);
                                }

				if(attr.sample_type & PERF_SAMPLE_STREAM_ID) {
					struct perf_record_sample_stream_id *id = sample;
					fprintf(out->file,"ID: %d\n",id->stream_id);
					sample += sizeof(*id);
				}
			
				if(attr.sample_type & PERF_SAMPLE_RAW) {
					struct perf_record_sample_raw *raw = sample;
					int i;
					printf("Size: %d %d\n",sizeof(struct sched_switch),raw->size);
					struct sched_switch *swtch = (struct sched_switch *)raw->data;
					printf("PrevPID: %d PrevComm: %s NextComm: %s NextPID:%d\n",
										swtch->prev_pid,
										swtch->prev_comm,
										swtch->next_comm,
										swtch->next_pid);

					fprintf(out->file,"\n");
					sample += sizeof(*raw);
				}
		};
		nread += header->size;
		buf   += header->size;
	}
	return nread;
		
}		
