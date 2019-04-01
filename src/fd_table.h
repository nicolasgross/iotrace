#ifndef IOTRACE_FD_TABLE_H
#define IOTRACE_FD_TABLE_H

#include <glib.h>
#include <stdbool.h>


typedef GHashTable *fd_table;

/**
 * Creates a new table that maps file descriptors to filenames.
 *
 * @return the mapping table.
 */
fd_table fd_table_create(void);

/**
 * Inserts a new file descriptor and filename into the table.
 *
 * @param table the mapping table.
 * @param fd the file descriptor.
 * @param filename the filename.
 */
void fd_table_insert(fd_table table, int fd, char const *filename);

/**
 * Inserts a duplicate into the table.
 *
 * @param table the mapping table.
 * @param orig_fd the original file descriptor.
 * @param dup_fd the new duplicate file descriptor.
 */
void fd_table_insert_dup(fd_table table, int orig_fd, int dup_fd);

/**
 * Removes a file descriptor and its corresponding file name from the table.
 *
 * @param table the mapping table.
 * @param fd the file descriptor.
 * @return true if file descriptor was found and removed.
 */
bool fd_table_remove(fd_table table, int fd);

/**
 * Returns the filename of a file descriptor.
 *
 * @param table the mapping table.
 * @param fd the file descriptor.
 * @return the filename, can be NULL if no mapping exists.
 */
char const *fd_table_lookup(fd_table table, int fd);

/**
 * Frees a file descriptor mapping table.
 *
 * @param table the table that should be freed.
 */
void fd_table_free(fd_table table);


#endif

