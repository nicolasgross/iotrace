#ifndef IOTRACE_JSON_PRINTER_H
#define IOTRACE_JSON_PRINTER_H

#include <glib.h>
#include <stdbool.h>

/**
 * Prints file statistics as JSON.
 *
 * @param filename the path/filename for the JSON file.
 * @param hostname the hostname to which the statistics belong.
 * @param rank the rank to which the statistics belong.
 * @return true if saving to the JSON file was successful.
 *
 */
bool print_stats_as_json(char const *filename, char const *hostname,
                         char const *rank);


#endif

