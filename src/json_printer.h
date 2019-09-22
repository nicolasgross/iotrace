#ifndef IOTRACE_JSON_PRINTER_H
#define IOTRACE_JSON_PRINTER_H

#include <glib.h>
#include <stdbool.h>

/**
 * Prints file statistics as JSON.
 *
 * @param filename the path/filename for the JSON file.
 * @param trace_id the trace id of the statistics.
 * @param hostname the hostname to which the statistics belong.
 * @param rank the rank to which the statistics belong.
 * @param syscall_table syscall statistics.
 * @return true if saving to the JSON file was successful.
 *
 */
bool print_stats_as_json(char const *filename, char const *trace_id,
                         char const *hostname, char const *rank,
                         GHashTable *syscall_table);


#endif

