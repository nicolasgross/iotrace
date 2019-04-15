#ifndef IOTRACE_FD_TABLE_H
#define IOTRACE_FD_TABLE_H

#include <glib.h>
#include <stdbool.h>


/**
 * Initalizes the table that maps file descriptors to filenames.
 *
 */
void fd_table_init(void);

/**
 * Inserts a new file descriptor and filename into the table.
 *
 * @param fd the file descriptor.
 * @param filename the filename.
 */
void fd_table_insert(int fd, char const *filename);

/**
 * Inserts a duplicate into the table.
 *
 * @param orig_fd the original file descriptor.
 * @param dup_fd the new duplicate file descriptor.
 */
void fd_table_insert_dup(int orig_fd, int dup_fd);

/**
 * Removes a file descriptor and its corresponding file name from the table.
 *
 * @param fd the file descriptor.
 * @return true if file descriptor was found and removed.
 */
bool fd_table_remove(int fd);

/**
 * Returns the filename of a file descriptor.
 *
 * @param fd the file descriptor.
 * @return the filename, can be NULL if no mapping exists.
 */
char const *fd_table_lookup(int fd);

/**
 * Frees the file descriptor mapping table.
 *
 */
void fd_table_free(void);


#endif

