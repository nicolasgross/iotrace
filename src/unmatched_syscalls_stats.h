#ifndef IOTRACE_UNMATCHED_SYSCALLS_STATS_H
#define IOTRACE_UNMATCHED_SYSCALLS_STATS_H

#include <glib.h>


typedef struct {
	unsigned long long count;
	unsigned long long total_ns;
} syscall_stat;


void syscall_stat_init(void);

void syscall_stat_free(void);

syscall_stat *syscall_stat_get(int const syscall);

GHashTable *syscall_stat_get_all(void);

void syscall_stat_incr(int const syscall, unsigned long long const time_ns);

void syscall_stat_print_all(void);


#endif

