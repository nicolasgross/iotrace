#include <glib.h>

#include "thread_temporaries.h"
#include "fd_table.h"


static GHashTable *thread_tmps_table;
static GMutex threads_tmps_mutex;


static void free_single_thread_tmps(gpointer tmps_ptr) {
	thread_tmps *tmps = tmps_ptr;
	if (g_atomic_int_dec_and_test(tmps->share_count)) {
		fd_table_free(tmps->fd_table);
		free(tmps->share_count);
		g_mutex_clear(tmps->fd_mutex);
		free(tmps->fd_mutex);
	}
	free(tmps);
}

void thread_tmps_init(void) {
	thread_tmps_table = g_hash_table_new_full(g_int_hash, g_int_equal, free,
	                                          free_single_thread_tmps);
}

void thread_tmps_insert(pid_t thread, GHashTable *const fd_table,
                        guint *const share_count, GMutex *const shared_fd_mutex) {
	int *pid_mem = malloc(sizeof(int));
	*pid_mem = thread;
	thread_tmps *tmps_mem = malloc(sizeof(thread_tmps));
	tmps_mem->fd_table = fd_table;
	if (share_count && shared_fd_mutex) {
		// fd table is shared at the moment
		tmps_mem->share_count = share_count;
		tmps_mem->fd_mutex = shared_fd_mutex;
	} else {
		// fd table is unshared at the moment
		tmps_mem->share_count = malloc(sizeof(guint));
		*tmps_mem->share_count = 0;
		tmps_mem->fd_mutex = malloc(sizeof(GMutex));
		g_mutex_init(tmps_mem->fd_mutex);
	}
	g_atomic_int_inc(tmps_mem->share_count);
	g_mutex_lock(&threads_tmps_mutex);
	g_hash_table_insert(thread_tmps_table, (gpointer) pid_mem, (gpointer) tmps_mem);
	g_mutex_unlock(&threads_tmps_mutex);
}

thread_tmps *thread_tmps_lookup(pid_t thread) {
	thread_tmps *tmps;
	g_mutex_lock(&threads_tmps_mutex);
	tmps = g_hash_table_lookup(thread_tmps_table, &thread);
	g_mutex_unlock(&threads_tmps_mutex);
	return tmps;
}

bool thread_tmps_remove(pid_t thread) {
	g_mutex_lock(&threads_tmps_mutex);
	bool retval = g_hash_table_remove(thread_tmps_table, &thread);
	g_mutex_unlock(&threads_tmps_mutex);
	return retval;
}

void thread_tmps_free(void) {
	g_hash_table_destroy(thread_tmps_table);
}

