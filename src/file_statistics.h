#ifndef IOTRACE_FILE_STATISTICS_H
#define IOTRACE_FILE_STATISTICS_H


typedef struct {
	unsigned long long count;           // number of opens/closes
	unsigned long long total_ns;        // total time in nanoseconds
	unsigned long long min_ns;          // min time in nanoseconds
	unsigned long long max_ns;          // max time in nanoseconds
} open_close_stat;

typedef struct {
	unsigned long long total_b;         // total number of bytes
	unsigned long long total_ns;        // total time in nanoseconds
	unsigned long long min_bps;         // min bytes per second
	unsigned long long max_bps;         // max bytes per second
} read_write_stat;

typedef struct {
	open_close_stat open_stats;
	open_close_stat close_stats;
	read_write_stat read_stats;
	read_write_stat write_stats;
	// TODO read: count + block size
	// TODO write: count + block size
} file_stat;


void file_stat_init(void); // Handle stdin, stdout, stderr

void file_stat_free(void);

file_stat *file_stat_get(char const *filename);

void file_stat_incr_open(char const *name, unsigned long long time_ns);

void file_stat_incr_close(char const *name, unsigned long long time_ns);

void file_stat_print_all(void);


#endif

