#include <glib.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "fd_table.h"


typedef struct {
	char *filename;
	bool cloexec;
} fd_table_entry;


static void fd_table_insert_intern(GHashTable *const fd_table, int fd,
                                   char const *const filename, bool cloexec) {
	int *fd_mem = malloc(sizeof(int));
	*fd_mem = fd;
	fd_table_entry *entry_mem = malloc(sizeof(fd_table_entry));
	size_t len = strlen(filename);
	entry_mem->filename = malloc(sizeof(char) * (len + 1));
	strcpy(entry_mem->filename, filename);
	entry_mem->cloexec = cloexec;
	g_hash_table_insert(fd_table, (gpointer) fd_mem, (gpointer) entry_mem);
}

static void free_single_fd_table_entry(gpointer entry) {
	fd_table_entry *entry_mem = entry;
	free(entry_mem->filename);
	free(entry_mem);
}

GHashTable *fd_table_create(void) {
	GHashTable *fd_table = g_hash_table_new_full(g_int_hash, g_int_equal, free,
	                                             free_single_fd_table_entry);
	// Add stdin, stdout, stderr to table, because they are initially open.
	fd_table_insert_intern(fd_table, 0, "stdin", false);
	fd_table_insert_intern(fd_table, 1, "stdout", false);
	fd_table_insert_intern(fd_table, 2, "stderr", false);
	return fd_table;
}

void fd_table_insert(GHashTable *const fd_table, GMutex *const fd_mutex,
                     int fd, char const *const filename, bool cloexec) {
	g_mutex_lock(fd_mutex);
	fd_table_insert_intern(fd_table, fd, filename, cloexec);
	g_mutex_unlock(fd_mutex);
}

void fd_table_insert_dup(GHashTable *const fd_table, GMutex *const fd_mutex,
                         int orig_fd, int dup_fd, bool cloexec) {
	g_mutex_lock(fd_mutex);
	fd_table_entry *entry = g_hash_table_lookup(fd_table, &orig_fd);
	if (cloexec) {
		fd_table_insert_intern(fd_table, dup_fd, entry->filename, true);
	} else {
		fd_table_insert_intern(fd_table, dup_fd, entry->filename, false);
	}
	g_mutex_unlock(fd_mutex);
}

void fd_table_set_cloexec(GHashTable *const fd_table, GMutex *const fd_mutex,
                          int fd, bool cloexec) {
	g_mutex_lock(fd_mutex);
	fd_table_entry *entry = g_hash_table_lookup(fd_table, &fd);
	entry->cloexec = cloexec;
	g_mutex_unlock(fd_mutex);
}

bool fd_table_remove(GHashTable *const fd_table, GMutex *const fd_mutex, int fd) {
	g_mutex_lock(fd_mutex);
	bool retval = g_hash_table_remove(fd_table, &fd);
	g_mutex_unlock(fd_mutex);
	return retval;
}

void fd_table_remove_cloexec(GHashTable *const fd_table, GMutex *const fd_mutex) {
	g_mutex_lock(fd_mutex);
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, fd_table);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		fd_table_entry const *const entry = value;
		if (entry->cloexec) {
			g_hash_table_iter_remove(&iter);
		}
	}
	g_mutex_unlock(fd_mutex);
}

GHashTable *fd_table_deep_copy(GHashTable *const fd_table, GMutex *const fd_mutex) {
	GHashTable *copy = g_hash_table_new_full(g_int_hash, g_int_equal, free,
	                                         free_single_fd_table_entry);

	g_mutex_lock(fd_mutex);
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, fd_table);
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		int const *const fd = key;
		fd_table_entry const *const entry = value;
		fd_table_insert_intern(copy, *fd, entry->filename, entry->cloexec);
	}
	g_mutex_unlock(fd_mutex);

	return copy;
}

char const *fd_table_lookup(GHashTable *const fd_table, int fd) {
	fd_table_entry *entry;
	if ((entry = ((fd_table_entry *) g_hash_table_lookup(fd_table, &fd)))) {
		return entry->filename;
	} else {
		return NULL;
	}
}

void fd_table_free(GHashTable *const fd_table) {
	g_hash_table_destroy(fd_table);
}

