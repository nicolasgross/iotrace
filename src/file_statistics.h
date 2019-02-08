#ifndef IOTRACE_FILE_STATISTICS_H
#define IOTRACE_FILE_STATISTICS_H


typedef struct {
	unsigned long long count;
	unsigned long long total_ns;
	unsigned long long min_ns;
	unsigned long long max_ns;
} open_close_stat;

typedef struct {
	open_close_stat open_stats;
	open_close_stat close_stats;
	// TODO read: count + block size, min time, max time
	// TODO write: count + block size, min time, max time
} file_stat;


void file_stat_init(void); // Handle stdin, stdout, stderr

void file_stat_free(void);

file_stat *file_stat_get(char const *filename);

void file_stat_incr_open(char const *name, unsigned long long time_ns);

void file_stat_incr_close(char const *name, unsigned long long time_ns);

void file_stat_print_all(void);


#endif

