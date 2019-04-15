#include <glib.h>

#include "thread_temporaries.h"


static GHashTable *thread_tmps_table;
static GMutex threads_tmps_mutex;


void thread_tmps_init(void) {
	thread_tmps_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
}

void thread_tmps_insert(pid_t thread) {
	int *pid_mem = malloc(sizeof(int));
	*pid_mem = thread;
	thread_tmps *tmps_mem = malloc(sizeof(thread_tmps));
	g_mutex_lock(&threads_tmps_mutex);
	g_hash_table_insert(thread_tmps_table, (gpointer) pid_mem, (gpointer) tmps_mem);
	g_mutex_unlock(&threads_tmps_mutex);
}

thread_tmps *thread_tmps_get(pid_t thread) {
	thread_tmps *tmps;
	g_mutex_lock(&threads_tmps_mutex);
	tmps = g_hash_table_lookup(thread_tmps_table, &thread);
	g_mutex_unlock(&threads_tmps_mutex);
	return tmps;
}

void thread_tmps_free(void) {
	g_hash_table_destroy(thread_tmps_table);
}

