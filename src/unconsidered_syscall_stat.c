#include <glib.h>
#include <stdio.h>

#include "unconsidered_syscall_stat.h"
#include "syscall_names.h"


GHashTable *syscall_stat_create(void) {
	return g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
}

void syscall_stat_free(GHashTable *syscall_table) {
	g_hash_table_destroy(syscall_table);
}

syscall_stat *syscall_stat_get(GHashTable *syscall_table, int const syscall) {
	return g_hash_table_lookup(syscall_table, &syscall);
}

static void syscall_stat_insert(GHashTable *syscall_table,
                                 int const syscall) {
	int *syscall_mem = malloc(sizeof(int));
	*syscall_mem = syscall;
	syscall_stat *stat_mem = malloc(sizeof(syscall_stat));
	stat_mem->count = 0;
	stat_mem->total_ns = 0;
	g_hash_table_insert(syscall_table, syscall_mem, stat_mem);
}

void syscall_stat_incr(GHashTable *syscall_table, int const syscall,
                       unsigned long long const time_ns) {
	syscall_stat *tmp = syscall_stat_get(syscall_table, syscall);
	if (tmp == NULL) {
		syscall_stat_insert(syscall_table, syscall);
		tmp = syscall_stat_get(syscall_table, syscall);
	}
	tmp->count++;
	tmp->total_ns += time_ns;
}

void syscall_stat_merge(GHashTable *syscall_table_1,
                        GHashTable *syscall_table_2) {
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, syscall_table_2);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		int *syscall = key;
		syscall_stat *stat_2 = value;
		syscall_stat *stat_1 = syscall_stat_get(syscall_table_1, *syscall);
		if (stat_1 == NULL) {
			syscall_stat_insert(syscall_table_1, *syscall);
			stat_1 = syscall_stat_get(syscall_table_1, *syscall);
		}
		stat_1->count += stat_2->count;
		stat_1->total_ns += stat_2->total_ns;
	}
}

void syscall_stat_print_all(GHashTable *syscall_table) {
	printf("UNCONSIDERED SYSCALLS IN FILE STATISTICS:\n\n");

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, syscall_table);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		syscall_stat *tmp = value;
		printf("%s  ->  count: %llu, total: %llu ns\n",
		       syscall_names[*(int *) key], tmp->count, tmp->total_ns);
	}
	printf("\n");
}

