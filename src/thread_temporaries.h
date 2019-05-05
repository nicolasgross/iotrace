#ifndef IOTRACE_THREAD_TEMPORARIES_H
#define IOTRACE_THREAD_TEMPORARIES_H

#include <time.h>
#include <glib.h>
#include <stdbool.h>

#define FILENAME_BUFF_SIZE 256


typedef struct {
	struct timespec start_time;
	void *ptr;
	int int_a;
	int int_b;
	char filename_buffer[FILENAME_BUFF_SIZE];
	GHashTable *fd_table;
	guint *share_count;
	GMutex *fd_mutex;
} thread_tmps;


void thread_tmps_init(void);

void thread_tmps_insert(pid_t thread, GHashTable *const shared_fd_table,
                        guint *const share_count, GMutex *const shared_fd_mutex);

thread_tmps *thread_tmps_lookup(pid_t thread);

bool thread_tmps_remove(pid_t thread);

void thread_tmps_free(void);


#endif

