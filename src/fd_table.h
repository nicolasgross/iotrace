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


#ifndef IOTRACE_FD_TABLE_H
#define IOTRACE_FD_TABLE_H

#include <glib.h>
#include <stdbool.h>


/**
 * Creates a table that maps file descriptors to filenames.
 *
 * @return fd_table a new file descriptor table.
 */
GHashTable *fd_table_create(void);

/**
 * Inserts a new file descriptor and filename into a table.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 * @param fd the file descriptor.
 * @param filename the filename.
 * @param cloexec the value of the cloexec flag.
 */
void fd_table_insert(GHashTable *const fd_table, GMutex *const fd_mutex,
                     int fd, char const *const filename, bool cloexec);

/**
 * Inserts a duplicate into a table.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 * @param orig_fd the original file descriptor.
 * @param dup_fd the new duplicate file descriptor.
 * @param cloexec the value of the cloexec flag.
 * @param slave whether the fd refers to a pseudoterminal peer slave.
 */
void fd_table_insert_dup(GHashTable *const fd_table, GMutex *const fd_mutex,
                         int orig_fd, int dup_fd, bool cloexec, bool slave);

/**
 * Sets the cloexec flag for a file descriptor.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 * @param fd the file descriptor.
 * @param cloexec the new value of the cloexec flag.
 */
void fd_table_set_cloexec(GHashTable *const fd_table, GMutex *const fd_mutex,
                          int fd, bool cloexec);

/**
 * Removes a file descriptor and its corresponding file name from a table.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 * @param fd the file descriptor.
 * @return true if file descriptor was found and removed.
 */
bool fd_table_remove(GHashTable *const fd_table, GMutex *const fd_mutex, int fd);

/**
 * Removes all file descriptors from a table that have the cloexec flag set.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 */
void fd_table_remove_cloexec(GHashTable *const fd_table, GMutex *const fd_mutex);

/**
 * Creates a deep copy of a file descriptor table.
 *
 * @param fd_table the file descriptor table that is to be copied.
 * @param fd_mutex the mutex for locking the table.
 * @return the (deep) copy of the given table.
 */
GHashTable *fd_table_deep_copy(GHashTable *const fd_table, GMutex *const fd_mutex);

/**
 * Returns the filename of a file descriptor. The caller must lock/unlock the
 * file descriptor table manually before calling this function and while
 * working with the returned filename.
 *
 * @param fd_table the file descriptor table.
 * @param fd_mutex the mutex for locking the table.
 * @param fd the file descriptor.
 * @return the filename, can be NULL if no mapping exists.
 */
char const *fd_table_lookup(GHashTable *const fd_table, int fd);

/**
 * Frees the file descriptor mapping table.
 *
 * @param fd_table the file descriptor table.
 */
void fd_table_free(GHashTable *const fd_table);


#endif

