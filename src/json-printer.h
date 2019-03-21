#include <glib.h>
#include <stdbool.h>

/**
 * Prints file statistics as JSON.
 *
 * @param stat_table the file statistics.
 * @param filename the path/filename for the JSON file.
 * @return true if saving to the JSON file was successful.
 *
 */
bool print_stats_as_json(GHashTable const *stat_table, char const *filename);

