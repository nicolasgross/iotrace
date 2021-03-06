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


#include <json-glib/json-glib.h>
#include <glib-object.h>
#include <stdbool.h>
#include <stdio.h>

#include "file_stat.h"
#include "unconsidered_syscall_stat.h"
#include "syscall_names.h"


static void builder_add_file_stats(JsonBuilder *builder,
                                   GHashTable *file_stat_table) {
	json_builder_set_member_name(builder, "file statistics");
	json_builder_begin_array(builder);

	GHashTableIter iter, blocks_iter;
	gpointer key, blocks_key;
	gpointer value, blocks_value;
	g_hash_table_iter_init (&iter, file_stat_table);
	file_stat *stat;
	while (g_hash_table_iter_next (&iter, &key, &value)) {
		stat = value;
		json_builder_begin_object(builder);

		// filename
		json_builder_set_member_name(builder, "filename");
		json_builder_add_string_value(builder, (char *) key);

		// open stats
		json_builder_set_member_name(builder, "open");
		json_builder_begin_array(builder);
		json_builder_add_int_value(builder, stat->open_stats.count);
		json_builder_add_int_value(builder, stat->open_stats.total_ns);
		json_builder_add_int_value(builder, stat->open_stats.min_ns);
		json_builder_add_int_value(builder, stat->open_stats.max_ns);
		json_builder_end_array(builder);

		// close stats
		json_builder_set_member_name(builder, "close");
		json_builder_begin_array(builder);
		json_builder_add_int_value(builder, stat->close_stats.count);
		json_builder_add_int_value(builder, stat->close_stats.total_ns);
		json_builder_add_int_value(builder, stat->close_stats.min_ns);
		json_builder_add_int_value(builder, stat->close_stats.max_ns);
		json_builder_end_array(builder);

		// read stats
		json_builder_set_member_name(builder, "read");
		json_builder_begin_array(builder);
		json_builder_add_int_value(builder, stat->read_stats.total_b);
		json_builder_add_int_value(builder, stat->read_stats.total_ns);
		json_builder_add_double_value(builder, stat->read_stats.min_bps);
		json_builder_add_double_value(builder, stat->read_stats.max_bps);
		json_builder_end_array(builder);

		// read blocks
		json_builder_set_member_name(builder, "read-blocks");
		json_builder_begin_array(builder);
		g_hash_table_iter_init (&blocks_iter, stat->read_stats.blocks);
		while (g_hash_table_iter_next (&blocks_iter, &blocks_key, &blocks_value)) {
			json_builder_begin_array(builder);
			json_builder_add_int_value(builder, *(ssize_t *) blocks_key);
			json_builder_add_int_value(builder, *(unsigned long *) blocks_value);
			json_builder_end_array(builder);
		}
		json_builder_end_array(builder);

		// write stats
		json_builder_set_member_name(builder, "write");
		json_builder_begin_array(builder);
		json_builder_add_int_value(builder, stat->write_stats.total_b);
		json_builder_add_int_value(builder, stat->write_stats.total_ns);
		json_builder_add_double_value(builder, stat->write_stats.min_bps);
		json_builder_add_double_value(builder, stat->write_stats.max_bps);
		json_builder_end_array(builder);

		// write blocks
		json_builder_set_member_name(builder, "write-blocks");
		json_builder_begin_array(builder);
		g_hash_table_iter_init (&blocks_iter, stat->write_stats.blocks);
		while (g_hash_table_iter_next (&blocks_iter, &blocks_key, &blocks_value)) {
			json_builder_begin_array(builder);
			json_builder_add_int_value(builder, *(ssize_t *) blocks_key);
			json_builder_add_int_value(builder, *(unsigned long *) blocks_value);
			json_builder_end_array(builder);
		}
		json_builder_end_array(builder);

		json_builder_end_object(builder);
	}

	json_builder_end_array(builder);
}

static void builder_add_syscall_stats(JsonBuilder *builder,
                                      GHashTable *syscall_table) {
	json_builder_set_member_name(builder, "unmatched syscalls");
	json_builder_begin_array(builder);

	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init(&iter, syscall_table);

	while (g_hash_table_iter_next (&iter, &key, &value)) {
		syscall_stat *tmp = value;
		json_builder_begin_object(builder);
		json_builder_set_member_name(builder, "syscall");
		json_builder_add_string_value(builder, syscall_names[*(int *) key]);
		json_builder_set_member_name(builder, "count");
		json_builder_add_int_value(builder, tmp->count);
		json_builder_set_member_name(builder, "total ns");
		json_builder_add_int_value(builder, tmp->total_ns);
		json_builder_end_object(builder);
	}

	json_builder_end_array(builder);
}

static JsonBuilder *create_builder(char const *trace_id, char const *hostname,
                                   char const *rank,
                                   GHashTable *file_stat_table,
                                   GHashTable *syscall_table) {
	// Initialize builder
	JsonBuilder *builder = json_builder_new();
	json_builder_begin_object(builder);

	json_builder_set_member_name(builder, "trace-id");
	json_builder_add_string_value(builder, (char *) trace_id);
	json_builder_set_member_name(builder, "hostname");
	json_builder_add_string_value(builder, (char *) hostname);
	json_builder_set_member_name(builder, "rank");
	json_builder_add_string_value(builder, (char *) rank ? rank : "NULL");

	builder_add_file_stats(builder, file_stat_table);
	builder_add_syscall_stats(builder, syscall_table);

	// Finalize builder
	json_builder_end_object(builder);
	return builder;
}

bool print_stats_as_json(char const *filename, char const *trace_id,
                         char const *hostname, char const *rank,
                         GHashTable *file_stat_table,
                         GHashTable *syscall_table) {
	JsonGenerator *gen = json_generator_new();
	json_generator_set_indent_char(gen, ' ');
	json_generator_set_indent(gen, 2);
	json_generator_set_pretty(gen, true);

	JsonBuilder *builder = create_builder(trace_id, hostname, rank,
	                                      file_stat_table, syscall_table);
	JsonNode *root = json_builder_get_root(builder);
	json_generator_set_root(gen, root);
	bool success = json_generator_to_file(gen, filename, NULL);

	json_node_free(root);
	g_object_unref(gen);
	g_object_unref(builder);

	return success;
}

