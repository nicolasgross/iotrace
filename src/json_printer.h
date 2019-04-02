#ifndef IOTRACE_JSON_PRINTER_H
#define IOTRACE_JSON_PRINTER_H

#include <glib.h>
#include <stdbool.h>

/**
 * Prints file statistics as JSON.
 *
 * @param filename the path/filename for the JSON file.
 * @return true if saving to the JSON file was successful.
 *
 */
bool print_stats_as_json(char const *filename);


#endif

