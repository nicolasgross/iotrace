#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "fd_table.h"


static void fd_table_insert_intern(GHashTable *const fd_table, int fd,
                                   char const *const filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	int *fd_mem = malloc(sizeof(int));
	*fd_mem = fd;
	g_hash_table_insert(fd_table, (gpointer) fd_mem, (gpointer) name_mem);
}

GHashTable *fd_table_create(void) {
	GHashTable *fd_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
	// Add stdin, stdout, stderr to table, because they are initially open.
	fd_table_insert_intern(fd_table, 0, "stdin");
	fd_table_insert_intern(fd_table, 1, "stdout");
	fd_table_insert_intern(fd_table, 2, "stderr");
	return fd_table;
}

void fd_table_insert(GHashTable *const fd_table, GMutex *const fd_mutex,
                     int fd, char const *const filename) {
	g_mutex_lock(fd_mutex);
	fd_table_insert_intern(fd_table, fd, filename);
	g_mutex_unlock(fd_mutex);
}

void fd_table_insert_dup(GHashTable *const fd_table, GMutex *const fd_mutex,
                         int orig_fd, int dup_fd) {
	g_mutex_lock(fd_mutex);
	char const *filename = fd_table_lookup(fd_table, orig_fd);
	fd_table_insert_intern(fd_table, dup_fd, filename);
	g_mutex_unlock(fd_mutex);
}

bool fd_table_remove(GHashTable *const fd_table, GMutex *const fd_mutex, int fd) {
	g_mutex_lock(fd_mutex);
	bool retval = g_hash_table_remove(fd_table, &fd);
	g_mutex_unlock(fd_mutex);
	return retval;
}

GHashTable *fd_table_deep_copy(GHashTable *const fd_table, GMutex *const fd_mutex) {
	GHashTable *copy = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);

	g_mutex_lock(fd_mutex);
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, fd_table);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		int const *const fd = key;
		char const *const filename = value;
		fd_table_insert_intern(copy, *fd, filename);
	}
	g_mutex_unlock(fd_mutex);

	return copy;
}

char const *fd_table_lookup(GHashTable *const fd_table, int fd) {
	return g_hash_table_lookup(fd_table, &fd);
}

void fd_table_free(GHashTable *const fd_table) {
	g_hash_table_destroy(fd_table);
}

