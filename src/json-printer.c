#include <json-glib/json-glib.h>
#include <glib-object.h>
#include <stdbool.h>

#include "file_statistics.h"


static JsonBuilder *create_builder(GHashTable *stat_table) {
	// Initialize builder
	JsonBuilder *builder = json_builder_new();
	json_builder_begin_object(builder);
	json_builder_set_member_name(builder, "statistics");
	json_builder_begin_array(builder);

	// Feed builder with file statistics
	GHashTableIter iter;
	gpointer key;
	gpointer value;
	g_hash_table_iter_init (&iter, stat_table);
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
		// TODO add block sizes and counts
		json_builder_end_array(builder);

		// write stats
		json_builder_set_member_name(builder, "write");
		json_builder_begin_array(builder);
		json_builder_add_int_value(builder, stat->write_stats.total_b);
		json_builder_add_int_value(builder, stat->write_stats.total_ns);
		json_builder_add_double_value(builder, stat->write_stats.min_bps);
		json_builder_add_double_value(builder, stat->write_stats.max_bps);
		// TODO add block sizes and counts
		json_builder_end_array(builder);

		json_builder_end_object(builder);
	}

	// Finalize builder
	json_builder_end_array(builder);
	json_builder_end_object(builder);
	return builder;
}

bool print_stats_as_json(GHashTable *stat_table, char const *filename) {
	JsonGenerator *gen = json_generator_new();
	json_generator_set_indent_char(gen, ' ');
	json_generator_set_indent(gen, 2);
	json_generator_set_pretty(gen, true);

	JsonBuilder *builder = create_builder(stat_table);
	JsonNode *root = json_builder_get_root(builder);
	json_generator_set_root(gen, root);
	bool success = json_generator_to_file(gen, filename, NULL);

	json_node_free(root);
	g_object_unref(gen);
	g_object_unref(builder);

	return success;
}

