#include <glib.h>

#include "unmatched_syscalls_stats.h"


static GHashTable *syscall_table;


void syscall_stat_init(void) {
	syscall_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
}

void syscall_stat_free(void) {
	g_hash_table_destroy(syscall_table);
}

syscall_stat *syscall_stat_get(int const syscall) {
	return g_hash_table_lookup(syscall_table, &syscall);
}

GHashTable *syscall_stat_get_all(void) {
	return syscall_table;
}

static void syscall_table_insert(int const syscall) {
	int *syscall_mem = malloc(sizeof(int));
	*syscall_mem = syscall;
	syscall_stat *stat_mem = malloc(sizeof(syscall_stat));
	stat_mem->count = 0;
	stat_mem->total_ns = 0;
	g_hash_table_insert(syscall_table, syscall_mem, stat_mem);
}

void syscall_stat_incr(int const syscall, unsigned long long const time_ns) {
	syscall_stat *tmp = syscall_stat_get(syscall);
	if (tmp == NULL) {
		syscall_table_insert(syscall);
		tmp = syscall_stat_get(syscall);
	}
	tmp->count++;
	tmp->total_ns += time_ns;
}

void syscall_stat_print_all(void) {
	// TODO
}

