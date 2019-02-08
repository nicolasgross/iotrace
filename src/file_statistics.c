#include <glib.h>

#include "file_statistics.h"


static GHashTable *file_stat_table;


void file_stat_init(); // Handle stdin, stdout, stderr

void file_stat_free();

void file_stat_incr_open(char *name, long long time_ns);

void file_stat_incr_close(char *name, long long time_ns);

file_stat file_stat_get(char const *filename);

