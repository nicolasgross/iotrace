/*
 * Copyright (C) 2019 HLRS, University of Stuttgart
 * <https://www.hlrs.de/>, <https://www.uni-stuttgart.de/>
 *
 * This file is part of iotrace.
 *
 * iotrace is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iotrace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iotrace.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The following people contributed to the project (in alphabetic order
 * by surname):
 *
 * - Nicolas Gross <https://github.com/nicolasgross>
 */


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

