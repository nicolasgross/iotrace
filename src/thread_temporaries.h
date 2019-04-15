#ifndef IOTRACE_THREAD_TEMPORARIES_H
#define IOTRACE_THREAD_TEMPORARIES_H

#include <time.h>

#define FILENAME_BUFF_SIZE 256


typedef struct {
	struct timespec start_time;
	int fd;
	int sc;
	char filename_buffer[FILENAME_BUFF_SIZE];
	int fcntl_cmd;
} thread_tmps;


void thread_tmps_init(void);

void thread_tmps_insert(pid_t thread);

thread_tmps *thread_tmps_get(pid_t thread);

void thread_tmps_free(void);


#endif

