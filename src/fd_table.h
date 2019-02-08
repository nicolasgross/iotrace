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
void fd_table_insert(fd_table table, int fd, char *filename);

/**
 * Removes a file descriptor and its corresponding file name from the table.
 *
 * @param table the mapping table.
 * @param fd the file descriptor.
 * @return
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

