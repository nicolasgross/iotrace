/*
 * Copyright (C) 2019 HLRS, University of Stuttgart
 * <https://www.hlrs.de/>, <https://www.uni-stuttgart.de/>
 *
 * This file is part of iotrace.
 *
 * iotrace is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * iotrace is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with iotrace.  If not, see <https://www.gnu.org/licenses/>.
 *
 * The following people contributed to the project (in alphabetic order
 * by surname):
 *
 * - Nicolas Gross <https://github.com/nicolasgross>
 */


#include <glib.h>
#include <stdio.h>

#include "file_stat.h"

#define MIN_NS_INITIAL 9999999999999U
#define MIN_BPS_INITIAL 9999999999999.0


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

static void stat_table_insert(GHashTable *file_stat_table,
                              char const *filename) {
	size_t len = strlen(filename);
	char *name_mem = malloc(sizeof(char) * (len + 1));
	strcpy(name_mem, filename);
	file_stat *stat_mem = malloc(sizeof(file_stat));
	init_file_stat(stat_mem);
	g_hash_table_insert(file_stat_table, name_mem, stat_mem);
}

static void free_single_file_stat(gpointer stat) {
	file_stat *tmp = stat;
	g_hash_table_destroy(tmp->read_stats.blocks);
	g_hash_table_destroy(tmp->write_stats.blocks);
	free(stat);
}

GHashTable *file_stat_create(void) {
	GHashTable *table = g_hash_table_new_full(g_str_hash, g_str_equal, free,
	                                          free_single_file_stat);
	stat_table_insert(table, "NULL");
	return table;
}

void file_stat_free(GHashTable *file_stat_table) {
	g_hash_table_destroy(file_stat_table);
}

file_stat *file_stat_get(GHashTable *file_stat_table, char const *filename) {
	return g_hash_table_lookup(file_stat_table, filename);
}

static file_stat *file_stat_get_safe(GHashTable *file_stat_table,
                                     char const *filename) {
	file_stat *tmp = file_stat_get(file_stat_table, filename);
	if (tmp == NULL) {
		stat_table_insert(file_stat_table, filename);
		tmp = file_stat_get(file_stat_table, filename);
	}
	return tmp;
}

void file_stat_incr_open(GHashTable *file_stat_table, char const *filename,
                         unsigned long long const time_ns) {
	file_stat *tmp = file_stat_get_safe(file_stat_table, filename);
	tmp->open_stats.count++;
	tmp->open_stats.total_ns += time_ns;
	if (tmp->open_stats.min_ns > time_ns) {
		tmp->open_stats.min_ns = time_ns;
	}
	if (tmp->open_stats.max_ns < time_ns) {
		tmp->open_stats.max_ns = time_ns;
	}
}

void file_stat_incr_close(GHashTable *file_stat_table, char const *filename,
                          unsigned long long const time_ns) {
	file_stat *tmp = file_stat_get_safe(file_stat_table, filename);
	tmp->close_stats.count++;
	tmp->close_stats.total_ns += time_ns;
	if (tmp->close_stats.min_ns > time_ns) {
		tmp->close_stats.min_ns = time_ns;
	}
	if (tmp->close_stats.max_ns < time_ns) {
		tmp->close_stats.max_ns = time_ns;
	}
}

static void file_stat_incr_rw(read_write_stat *stat, unsigned long long const time_ns,
                              ssize_t const bytes) {
	stat->total_ns += time_ns;
	if (bytes > 0) {
		stat->total_b += bytes;
		double time_s = time_ns / 1000000000.0;
		double bps = bytes / time_s;
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

void file_stat_incr_read(GHashTable *file_stat_table, char const *filename,
                         unsigned long long const time_ns,
                         ssize_t const bytes) {
	file_stat *tmp = file_stat_get_safe(file_stat_table, filename);
	file_stat_incr_rw(&tmp->read_stats, time_ns, bytes);
}

void file_stat_incr_write(GHashTable *file_stat_table, char const *filename,
                          unsigned long long const time_ns,
                          ssize_t const bytes) {
	file_stat *tmp = file_stat_get_safe(file_stat_table, filename);
	file_stat_incr_rw(&tmp->write_stats, time_ns, bytes);
}

static void merge_open_close(open_close_stat *stat_1, open_close_stat *stat_2) {
	stat_1->count += stat_2->count;
	if (stat_1->min_ns > stat_2->min_ns) {
		stat_1->min_ns = stat_2->min_ns;
	}
	if (stat_1->max_ns < stat_2->max_ns) {
		stat_1->max_ns = stat_2->max_ns;
	}
	stat_1->total_ns += stat_2->total_ns;
}

static void merge_read_write(read_write_stat *stat_1, read_write_stat *stat_2) {
	stat_1->total_b += stat_2->total_b;
	stat_1->total_ns += stat_2->total_ns;
	if (stat_1->min_bps > stat_2->min_bps) {
		stat_1->min_bps = stat_2->min_bps;
	}
	if (stat_1->max_bps < stat_2->max_bps) {
		stat_1->max_bps = stat_2->max_bps;
	}
}

static void merge_blocks(GHashTable *blocks_1, GHashTable *blocks_2) {
	GHashTableIter subiter;
	gpointer subkey;
	gpointer subvalue;
	g_hash_table_iter_init (&subiter, blocks_2);
	while (g_hash_table_iter_next (&subiter, &subkey, &subvalue)) {
		ssize_t *bytes = subkey;
		unsigned long *count_2 = subvalue;
		unsigned long *count_1 = g_hash_table_lookup(blocks_1, bytes);
		if (count_1 == NULL) {
			ssize_t *key = malloc(sizeof(ssize_t));
			*key = *bytes;
			unsigned long *value = malloc(sizeof(unsigned long));
			*value = 0;
			g_hash_table_insert(blocks_1, key, value);
			count_1 = value;
		}
		*count_1 += *count_2;
	}
}

void file_state_merge(GHashTable *file_stat_table_1,
                      GHashTable *file_stat_table_2) {
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, file_stat_table_2);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		char *file = key;
		file_stat *stat_2 = value;
		file_stat *stat_1 = file_stat_get(file_stat_table_1, file);
		if (stat_1 == NULL) {
			stat_table_insert(file_stat_table_1, file);
			stat_1 = file_stat_get(file_stat_table_1, file);
		}

		merge_open_close(&stat_1->open_stats, &stat_2->open_stats);
		merge_open_close(&stat_1->close_stats, &stat_2->close_stats);

		merge_read_write(&stat_1->read_stats, &stat_2->read_stats);
		merge_blocks(stat_1->read_stats.blocks, stat_2->read_stats.blocks);

		merge_read_write(&stat_1->write_stats, &stat_2->write_stats);
		merge_blocks(stat_1->write_stats.blocks, stat_2->write_stats.blocks);
	}
}

void file_stat_print_all(GHashTable *file_stat_table) {
	printf("FILE STATISTICS:\n\n");

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, file_stat_table);

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

