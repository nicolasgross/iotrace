#include <glib.h>
#include <stdio.h>

#include "file_stat.h"

#define MIN_NS_INITIAL 9999999999999U
#define MIN_BPS_INITIAL 9999999999999.0


static GHashTable *stat_table;
static GMutex stats_mutex;


static void init_file_stat(file_stat *stat) {
	stat->open_stats.count = 0;
	stat->open_stats.total_ns = 0;
	stat->open_stats.min_ns = MIN_NS_INITIAL;
	stat->open_stats.max_ns = 0;

	stat->close_stats.count = 0;
	stat->close_stats.total_ns = 0;
	stat->close_stats.min_ns = MIN_NS_INITIAL;
	stat->close_stats.max_ns = 0;

	stat->read_stats.total_b = 0;
	stat->read_stats.total_ns = 0;
	stat->read_stats.min_bps = MIN_BPS_INITIAL;
	stat->read_stats.max_bps = 0.0;
	stat->read_stats.blocks = g_hash_table_new_full(g_int_hash, g_int_equal,
	                                                free, free);

	stat->write_stats.total_b = 0;
	stat->write_stats.total_ns = 0;
	stat->write_stats.min_bps = MIN_BPS_INITIAL;
	stat->write_stats.max_bps = 0.0;
	stat->write_stats.blocks = g_hash_table_new_full(g_int_hash, g_int_equal,
	                                                 free, free);
}

static void stat_table_insert(char const *filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	file_stat *stat_mem = malloc(sizeof(file_stat));
	init_file_stat(stat_mem);
	g_hash_table_insert(stat_table, name_mem, stat_mem);
}

static void free_single_file_stat(gpointer stat) {
	file_stat *tmp = stat;
	g_hash_table_destroy(tmp->read_stats.blocks);
	g_hash_table_destroy(tmp->write_stats.blocks);
	free(stat);
}

void file_stat_init(void) {
	stat_table = g_hash_table_new_full(g_str_hash, g_str_equal, free,
	                                   free_single_file_stat);
	stat_table_insert("NULL");
}

void file_stat_free(void) {
	g_hash_table_destroy(stat_table);
}

file_stat *file_stat_get(char const *filename) {
	return g_hash_table_lookup(stat_table, filename);
}

GHashTable *file_stat_get_all(void) {
	return stat_table;
}

static file_stat *file_stat_get_safe(char const *filename) {
	file_stat *tmp = file_stat_get(filename);
	if (tmp == NULL) {
		stat_table_insert(filename);
		tmp = file_stat_get(filename);
	}
	return tmp;
}

void file_stat_incr_open(char const *filename, unsigned long long const time_ns) {
	g_mutex_lock(&stats_mutex);
	file_stat *tmp = file_stat_get_safe(filename);
	tmp->open_stats.count++;
	tmp->open_stats.total_ns += time_ns;
	if (tmp->open_stats.min_ns > time_ns) {
		tmp->open_stats.min_ns = time_ns;
	}
	if (tmp->open_stats.max_ns < time_ns) {
		tmp->open_stats.max_ns = time_ns;
	}
	g_mutex_unlock(&stats_mutex);
}

void file_stat_incr_close(char const *filename, unsigned long long const time_ns) {
	g_mutex_lock(&stats_mutex);
	file_stat *tmp = file_stat_get_safe(filename);
	tmp->close_stats.count++;
	tmp->close_stats.total_ns += time_ns;
	if (tmp->close_stats.min_ns > time_ns) {
		tmp->close_stats.min_ns = time_ns;
	}
	if (tmp->close_stats.max_ns < time_ns) {
		tmp->close_stats.max_ns = time_ns;
	}
	g_mutex_unlock(&stats_mutex);
}

static void file_stat_incr_rw(read_write_stat *stat, unsigned long long const time_ns,
                              ssize_t const bytes) {
	stat->total_ns += time_ns;
	if (bytes > 0) {
		stat->total_b += bytes;
		double time_s = time_ns / 1000000000.0;
		double factor = 1.0 / time_s;
		double bps = bytes * factor;
		if (stat->min_bps > bps) {
			stat->min_bps = bps;
		}
		if (stat->max_bps < bps) {
			stat->max_bps = bps;
		}
	}
	unsigned long *count = g_hash_table_lookup(stat->blocks, &bytes);
	if (count == NULL) {
		ssize_t *key = malloc(sizeof(ssize_t));
		*key = bytes;
		unsigned long *value = malloc(sizeof(unsigned long));
		*value = 0;
		g_hash_table_insert(stat->blocks, key, value);
		count = value;
	}
	*count += 1;
}

void file_stat_incr_read(char const *filename, unsigned long long const time_ns,
                         ssize_t const bytes) {
	g_mutex_lock(&stats_mutex);
	file_stat *tmp = file_stat_get_safe(filename);
	file_stat_incr_rw(&tmp->read_stats, time_ns, bytes);
	g_mutex_unlock(&stats_mutex);
}

void file_stat_incr_write(char const *filename, unsigned long long const time_ns,
                          ssize_t const bytes) {
	g_mutex_lock(&stats_mutex);
	file_stat *tmp = file_stat_get_safe(filename);
	file_stat_incr_rw(&tmp->write_stats, time_ns, bytes);
	g_mutex_unlock(&stats_mutex);
}

void file_stat_print_all(void) {
	printf("FILE STATISTICS:\n\n");

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

		printf("      blocks [bytes, count] -> ");
		GHashTableIter subiter;
		gpointer subkey;
		gpointer subvalue;
		g_hash_table_iter_init (&subiter, tmp->read_stats.blocks);
		while (g_hash_table_iter_next (&subiter, &subkey, &subvalue)) {
			printf("[%ld, %ld], ", *(unsigned long *) subkey,
			       *(ssize_t *) subvalue);
		}
		printf("\n");

		printf("  write -> total bytes: %llu, total time: %llu ns, "
		       "min: %f byte/s, max: %f byte/s\n",
		       tmp->write_stats.total_b, tmp->write_stats.total_ns,
		       tmp->write_stats.min_bps, tmp->write_stats.max_bps);

		printf("      blocks [bytes, count] -> ");
		g_hash_table_iter_init (&subiter, tmp->write_stats.blocks);
		while (g_hash_table_iter_next (&subiter, &subkey, &subvalue)) {
			printf("[%ld, %ld], ", *(unsigned long *) subkey,
			       *(ssize_t *) subvalue);
		}
		printf("\n");
	}
	printf("\n");
}

