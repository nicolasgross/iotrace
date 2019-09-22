#ifndef IOTRACE_UNMATCHED_SYSCALLS_STATS_H
#define IOTRACE_UNMATCHED_SYSCALLS_STATS_H

#include <glib.h>


typedef struct {
	unsigned long long count;
	unsigned long long total_ns;
} syscall_stat;


GHashTable *syscall_stat_create(void);

void syscall_stat_free(GHashTable *syscall_table);

syscall_stat *syscall_stat_get(GHashTable *syscall_table, int const syscall);

void syscall_stat_incr(GHashTable *syscall_table, int const syscall,
                       unsigned long long const time_ns);

void syscall_stat_merge(GHashTable *syscall_table_1,
                        GHashTable *syscall_table_2);

void syscall_stat_print_all(GHashTable *syscall_table);


#endif

