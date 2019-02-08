#include <glib.h>

#include "file_statistics.h"


static GHashTable *stat_table;


static void init_file_stat(file_stat *stat) {
	stat->open_stats.count = 0;
	stat->open_stats.total_ns = 0;
	stat->open_stats.min_ns = ULLONG_MAX;
	stat->open_stats.max_ns = 0;

	stat->close_stats.count = 0;
	stat->close_stats.total_ns = 0;
	stat->close_stats.min_ns = ULLONG_MAX;
	stat->close_stats.max_ns = 0;

	// TODO other stats
}

static void stat_table_insert(char *filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	file_stat *stat_mem = malloc(sizeof(file_stat));
	init_file_stat(stat_mem);
	g_hash_table_insert(stat_table, name_mem, stat_mem);
}

void file_stat_init(void) {
	stat_table = g_hash_table_new_full(g_str_hash, g_str_equal, free, free);
}

void file_stat_free(void) {
	g_hash_table_destroy(stat_table);
}

file_stat *file_stat_get(char const *filename) {
	return g_hash_table_lookup(stat_table, filename);
}

void file_stat_incr_open(char *filename, unsigned long long time_ns) {
	file_stat *tmp = file_stat_get(filename);
	if (tmp == NULL) {
		stat_table_insert(filename);
		tmp = file_stat_get(filename);
	}
	tmp->open_stats.count++;
	tmp->open_stats.total_ns += time_ns;
	if (tmp->open_stats.min_ns > time_ns) {
		tmp->open_stats.min_ns = time_ns;
	}
	if (tmp->open_stats.max_ns < time_ns) {
		tmp->open_stats.max_ns = time_ns;
	}
}

void file_stat_incr_close(char *filename, unsigned long long time_ns) {
	file_stat *tmp = file_stat_get(filename);
	if (tmp == NULL) {
		stat_table_insert(filename);
		tmp = file_stat_get(filename);
	}
	tmp->close_stats.count++;
	tmp->close_stats.total_ns += time_ns;
	if (tmp->close_stats.min_ns > time_ns) {
		tmp->close_stats.min_ns = time_ns;
	}
	if (tmp->close_stats.max_ns < time_ns) {
		tmp->close_stats.max_ns = time_ns;
	}
}

