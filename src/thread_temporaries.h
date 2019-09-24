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
	int int_c;
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

