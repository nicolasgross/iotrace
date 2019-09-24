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
 * @param file_stat_table file statistics.
 * @param syscall_table syscall statistics.
 * @return true if saving to the JSON file was successful.
 *
 */
bool print_stats_as_json(char const *filename, char const *trace_id,
                         char const *hostname, char const *rank,
                         GHashTable *file_stat_table,
                         GHashTable *syscall_table);


#endif

