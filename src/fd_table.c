#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "fd_table.h"


fd_table fd_table_create(void) {
	GHashTable *table = g_hash_table_new_full(g_int_hash,
	                                          g_int_equal, free, free);
	// Add stdin, stdout, stderr to table, because they are initially open.
	fd_table_insert(table, 0, "stdin");
	fd_table_insert(table, 1, "stdout");
	fd_table_insert(table, 2, "stderr");
	return table;
}

void fd_table_insert(fd_table table, int fd, char *filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	int *fd_mem = malloc(sizeof(int));
	*fd_mem = fd;
	g_hash_table_insert(table, (gpointer) fd_mem, (gpointer) name_mem);
}

bool fd_table_remove(fd_table table, int fd) {
	return (bool) g_hash_table_remove(table, &fd);
}

char const *fd_table_lookup(fd_table table, int fd) {
	return g_hash_table_lookup(table, &fd);
}

void fd_table_free(fd_table table) {
	g_hash_table_destroy(table);
}

