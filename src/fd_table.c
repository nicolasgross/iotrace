#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "fd_table.h"


static GHashTable *fd_table;
static GMutex fd_mutex;

void fd_table_init(void) {
	fd_table = g_hash_table_new_full(g_int_hash, g_int_equal, free, free);
	// Add stdin, stdout, stderr to table, because they are initially open.
	fd_table_insert(0, "stdin");
	fd_table_insert(1, "stdout");
	fd_table_insert(2, "stderr");
}

void fd_table_insert(int fd, char const *filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	int *fd_mem = malloc(sizeof(int));
	*fd_mem = fd;
	g_hash_table_insert(fd_table, (gpointer) fd_mem, (gpointer) name_mem);
}

void fd_table_insert_dup(int orig_fd, int dup_fd) {
	char const *filename = fd_table_lookup(fd_table, orig_fd);
	fd_table_insert(fd_table, dup_fd, filename);
}

bool fd_table_remove(int fd) {
	return (bool) g_hash_table_remove(fd_table, &fd);
}

char const *fd_table_lookup(int fd) {
	return g_hash_table_lookup(fd_table, &fd);
}

void fd_table_free(void) {
	g_hash_table_destroy(fd_table);
}

