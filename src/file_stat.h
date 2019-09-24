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


#ifndef IOTRACE_FILE_STATS_H
#define IOTRACE_FILE_STATS_H

#include <glib.h>
#include <unistd.h>


typedef struct {
	unsigned long long count;           // number of opens/closes
	unsigned long long total_ns;        // total time in nanoseconds
	unsigned long long min_ns;          // min time in nanoseconds
	unsigned long long max_ns;          // max time in nanoseconds
} open_close_stat;

typedef struct {
	unsigned long long total_b;         // total number of bytes
	unsigned long long total_ns;        // total time in nanoseconds
	double min_bps;                     // min bytes per second
	double max_bps;                     // max bytes per second
	GHashTable *blocks;                 // block sizes and their count
} read_write_stat;

typedef struct {
	open_close_stat open_stats;
	open_close_stat close_stats;
	read_write_stat read_stats;
	read_write_stat write_stats;
} file_stat;


GHashTable *file_stat_create(void);

void file_stat_free(GHashTable *file_stat_table);

file_stat *file_stat_get(GHashTable *file_stat_table, char const *filename);

void file_stat_incr_open(GHashTable *file_stat_table, char const *filename,
                         unsigned long long const time_ns);

void file_stat_incr_close(GHashTable *file_stat_table, char const *filename,
                          unsigned long long const time_ns);

void file_stat_incr_read(GHashTable *file_stat_table, char const *filename,
                         unsigned long long const time_ns,
                         ssize_t const bytes);

void file_stat_incr_write(GHashTable *file_stat_table, char const *filename,
                          unsigned long long const time_ns,
                          ssize_t const bytes);

void file_state_merge(GHashTable *file_stat_table_1,
                      GHashTable *file_stat_table_2);

void file_stat_print_all(GHashTable *file_stat_table);


#endif

