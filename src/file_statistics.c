#include <glib.h>
#include <stdio.h>

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

	stat->read_stats.total_b = 0;
	stat->read_stats.total_ns = 0;
	stat->read_stats.min_bps = 9999999999999.0;
	stat->read_stats.max_bps = 0.0;

	stat->write_stats.total_b = 0;
	stat->write_stats.total_ns = 0;
	stat->write_stats.min_bps = 9999999999999.0;
	stat->write_stats.max_bps = 0.0;

	// TODO other stats
}

static void stat_table_insert(char const *filename) {
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

static file_stat *file_stat_get_safe(char const *filename) {
	file_stat *tmp = file_stat_get(filename);
	if (tmp == NULL) {
		stat_table_insert(filename);
		tmp = file_stat_get(filename);
	}
	return tmp;
}

void file_stat_incr_open(char const *filename, unsigned long long time_ns) {
	file_stat *tmp = file_stat_get_safe(filename);
	tmp->open_stats.count++;
	tmp->open_stats.total_ns += time_ns;
	if (tmp->open_stats.min_ns > time_ns) {
		tmp->open_stats.min_ns = time_ns;
	}
	if (tmp->open_stats.max_ns < time_ns) {
		tmp->open_stats.max_ns = time_ns;
	}
}

void file_stat_incr_close(char const *filename, unsigned long long time_ns) {
	file_stat *tmp = file_stat_get_safe(filename);
	tmp->close_stats.count++;
	tmp->close_stats.total_ns += time_ns;
	if (tmp->close_stats.min_ns > time_ns) {
		tmp->close_stats.min_ns = time_ns;
	}
	if (tmp->close_stats.max_ns < time_ns) {
		tmp->close_stats.max_ns = time_ns;
	}
}

void file_stat_incr_read(char const *filename, unsigned long long time_ns,
                         ssize_t bytes) {
	file_stat *tmp = file_stat_get_safe(filename);
	tmp->read_stats.total_ns += time_ns; // TODO reicht wertebereich
	if (bytes > 0) {
		tmp->read_stats.total_b += bytes;
		double time_s = time_ns / 1000000000.0;
		double factor = 1 / time_s;
		double bps = bytes * factor;
		if (tmp->read_stats.min_bps > bps) {
			tmp->read_stats.min_bps = bps;
		}
		if (tmp->read_stats.max_bps < bps) {
			tmp->read_stats.max_bps = bps;
		}
	}
}

void file_stat_print_all(void) {
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, stat_table);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		file_stat *tmp = value;
		printf("File: %s\n", (char *) key);
		printf("  open  -> count: %llu, total: %llu ns, min: %llu ns, "
		       "max: %llu ns\n",
		       tmp->open_stats.count, tmp->open_stats.total_ns,
		       tmp->open_stats.min_ns, tmp->open_stats.max_ns);
		printf("  close -> count: %llu, total: %llu ns, min: %llu ns, "
		       "max: %llu ns\n",
		       tmp->close_stats.count, tmp->close_stats.total_ns,
		       tmp->close_stats.min_ns, tmp->close_stats.max_ns);
		printf("  read  -> total bytes: %llu, total time: %llu ns, "
		       "min: %f byte/s, max: %f byte/s\n",
		       tmp->read_stats.total_b, tmp->read_stats.total_ns,
		       tmp->read_stats.min_bps, tmp->read_stats.max_bps);
		printf("  write -> total bytes: %llu, total time: %llu ns, "
		       "min: %f byte/s, max: %f byte/s\n",
		       tmp->write_stats.total_b, tmp->write_stats.total_ns,
		       tmp->write_stats.min_bps, tmp->write_stats.max_bps);
		printf("\n");
	}
}

