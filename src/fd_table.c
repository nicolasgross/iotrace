#include <glib.h>

#include "fd_table.h"
#include "stdbool.h"


fd_table fd_table_create(void) {
	GHashTable *table = g_hash_table_new_full(g_int_hash,
	                                          g_int_equal, free, free);
	// Add stdin, stdout, stderr to mapping table, because they are
	// initially open.
	int *fd = malloc(sizeof(int));
	*fd = 0;
	char *name = malloc(sizeof(char) * 6);
	strncpy(name, "stdin", 6);
	fd_table_insert(table, fd, name);

	fd = malloc(sizeof(int));
	*fd = 1;
	name = malloc(sizeof(char) * 7);
	strncpy(name, "stdout", 7);
	fd_table_insert(table, fd, name);


	fd = malloc(sizeof(int));
	*fd = 2;
	name = malloc(sizeof(char) * 7);
	strncpy(name, "stderr", 7);
	fd_table_insert(table, fd, name);

	return table;
}

bool fd_table_insert(fd_table table, int *fd, char *filename) {
	return (bool) g_hash_table_insert(table, (gpointer) fd,
	                                  (gpointer) filename);
}

bool fd_table_remove(fd_table table, int fd) {
	return (bool) g_hash_table_remove(table, &fd);
}

char const *const fd_table_map(fd_table table, int fd) {
	return g_hash_table_lookup(table, &fd);
}

void fd_table_free(fd_table table) {
	g_hash_table_destroy(table);
}

