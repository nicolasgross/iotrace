#ifndef IOTRACE_FILE_STATS_H
#define IOTRACE_FILE_STATS_H

#include <glib.h>
#include <unistd.h>


typedef struct {
	unsigned long long count;           // number of opens/closes
	unsigned long long total_ns;        // total time in nanoseconds
	unsigned long long min_ns;          // min time in nanoseconds
	unsigned long long max_ns;          // max time in nanoseconds
} open_close_stat;

typedef struct {
	unsigned long long total_b;         // total number of bytes
	unsigned long long total_ns;        // total time in nanoseconds
	double min_bps;                     // min bytes per second
	double max_bps;                     // max bytes per second
	GHashTable *blocks;                 // block sizes and their count
} read_write_stat;

typedef struct {
	open_close_stat open_stats;
	open_close_stat close_stats;
	read_write_stat read_stats;
	read_write_stat write_stats;
} file_stat;


void file_stat_init(void);

void file_stat_free(void);

file_stat *file_stat_get(char const *filename);

GHashTable *file_stat_get_all(void);

void file_stat_incr_open(char const *filename, unsigned long long const time_ns);

void file_stat_incr_close(char const *filename, unsigned long long const time_ns);

void file_stat_incr_read(char const *filename, unsigned long long const time_ns,
                         ssize_t const bytes);

void file_stat_incr_write(char const *filename, unsigned long long const time_ns,
                          ssize_t const bytes);

void file_stat_print_all(void);


#endif
