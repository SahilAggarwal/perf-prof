#pragma once
#ifndef _H_OUTPUT
#define _H_OUTPUT

#include <linux/perf_event.h>
#include "buffer.h"

struct output_records {
	struct buf_reader 	reader;
	struct perf_event_attr 	attr;
	FILE			*file;
	int			e_type;
};

struct sched_switch {
	unsigned short 	common_type;
	unsigned char 	common_flags;
	unsigned char  	common_preempt_count;
	int 		common_pid;
	char	 	prev_comm[16];
	pid_t		prev_pid;
	int 		prev_prio;
	long		prev_state;
	char		next_comm[16];
	pid_t		next_pid;
	int		next_prio;
};

struct perf_record_sample_tid {
	__u32 pid;
	__u32 tid;
};

struct perf_record_sample_stream_id {
	__u64 stream_id;
};

struct perf_record_sample_time {
	__u64 time;
};

struct perf_record_sample_raw {
	__u32 size;
	char data[];
};

__u64 write_output(void *buf, __u64 size, struct output_records *);

#endif
