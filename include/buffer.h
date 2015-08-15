#pragma once
#ifndef _H_BUFFER
#define _H_BUFFER

#include <sys/types.h>

struct buf_reader {
	int (*read)(void *buf, __u64 size,void *buf_reader);
};

//inline int buf_read(void *buf, int size, struct buf_reader *buf_reader) {
//	return buf_reader->read(buf, size, buf_reader);
//}

#endif
