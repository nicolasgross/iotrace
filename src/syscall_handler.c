#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <linux/fcntl.h>
#include <time.h>
#include <errno.h>
#include <glib.h>
#include <unistd.h>

#include "syscall_handler.h"
#include "syscall_names.h"
#include "fd_table.h"
#include "file_stat.h"
#include "unconsidered_syscall_stat.h"
#include "thread_temporaries.h"

#define NANOS 1000000000LL

#ifdef CLOCK_MONOTONIC_RAW
	#define USED_CLOCK CLOCK_MONOTONIC_RAW
	#pragma message ("Compiled with clock id CLOCK_MONOTONIC_RAW")
#else
	#define USED_CLOCK CLOCK_MONOTONIC
	#pragma message ("Compiled with clock id CLOCK_MONOTONIC")
#endif


static int read_string(pid_t tracee, unsigned long base, char *dest,
                       const size_t max_len) {
	for (size_t i = 0; i * 8 < max_len; i++) {
		errno = 0;
		long data = ptrace(PTRACE_PEEKDATA, tracee, base + (i * sizeof(long)), NULL);
		if (data == -1 && errno != 0) {
			return -1;
		}
		memcpy(dest + (i * sizeof(long)), &data, sizeof(data));
		if (dest[i] == 0 || dest[i + 1] == 0 || dest[i + 2] == 0 ||
		    dest[i + 3] == 0 || dest[i + 4] == 0 || dest[i + 5] == 0 ||
		    dest[i + 6] == 0 || dest[i + 7] == 0) {
			return 0;
		}
	}
	dest[max_len - 1] = 0;
	return 0;
}

static unsigned long long calc_elapsed_ns(struct timespec *start_time,
                                          struct timespec *current_time) {
	unsigned long long start_ns = start_time->tv_sec * NANOS +
	                              start_time->tv_nsec;
	unsigned long long current_ns = current_time->tv_sec * NANOS +
	                                current_time->tv_nsec;
	return current_ns - start_ns;
}


// ---- open ----

static void handle_open_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	long base = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	if (read_string(tracee, base, tmps->filename_buffer, FILENAME_BUFF_SIZE)) {
		fprintf(stderr, "%s", "Error while reading filename of open");
		exit(1);
	}
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of open");
		exit(1);
	}
}

static void handle_open_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of open/openat");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (tmps->fd >= 0) {
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
		                tmps->filename_buffer);
	}
	file_stat_incr_open(tmps->filename_buffer, elapsed_ns);
}


// ---- openat ----

static void handle_openat_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	long base = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	if (read_string(tracee, base, tmps->filename_buffer, FILENAME_BUFF_SIZE)) {
		fprintf(stderr, "%s", "Error while reading filename of openat");
		exit(1);
	}
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of openat");
		exit(1);
	}
}


// ---- close ----

static void handle_close_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->fd = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of close");
		exit(1);
	}
}

static void handle_close_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of close");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->fd);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_close(filename, elapsed_ns);
	g_mutex_unlock(tmps->fd_mutex);
	fd_table_remove(tmps->fd_table, tmps->fd_mutex, tmps->fd);
}


// ---- read ----

static void handle_read_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->fd = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
    // TODO auslagern in eigene Funktion?
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of read");
		exit(1);
	}
}

static void handle_read_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of read");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	ssize_t ret_bytes = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->fd);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_read(filename, elapsed_ns, ret_bytes);
	g_mutex_unlock(tmps->fd_mutex);
}


// ---- write ----

static void handle_write_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->fd = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of write");
		exit(1);
	}
}

static void handle_write_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of write");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	ssize_t ret_bytes = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->fd);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_write(filename, elapsed_ns, ret_bytes);
	g_mutex_unlock(tmps->fd_mutex);
}


// ---- pipe/pipe2 ----

static void handle_pipe_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of pipe/pipe2");
		exit(1);
	}
}

static void handle_pipe_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of pipe/pipe2");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	if (ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX) == 0) {
		int *pipefd = (int *) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
		int pipefd_read = ptrace(PTRACE_PEEKDATA, tracee, pipefd, NULL);
		int pipefd_write = ptrace(PTRACE_PEEKDATA, tracee, pipefd + 1, NULL);
		#define PIPE_R "pipe_r"
		#define PIPE_W "pipe_w"
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_read, PIPE_R);
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_write, PIPE_W);
		file_stat_incr_open(PIPE_R, elapsed_ns/2);
		file_stat_incr_open(PIPE_W, elapsed_ns/2);
	}
}


// ==== UNCONSIDERED SYSCALLS ====

// ---- dup/dup2/dup3 ----

static void handle_dup_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->fd = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
}

static void handle_dup_return(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (tmps->fd >= 0) {
		fd_table_insert_dup(tmps->fd_table, tmps->fd_mutex, tmps->fd, ret_fd);
	}
}


// ---- fcntl ----

static void handle_fcntl_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	int cmd = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	tmps->fcntl_cmd = cmd;
	if (cmd == F_DUPFD || cmd == F_DUPFD_CLOEXEC) {
		handle_dup_call(tracee);
	}
}

static void handle_fcntl_return(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (tmps->fcntl_cmd == F_DUPFD || tmps->fcntl_cmd == F_DUPFD_CLOEXEC) {
		handle_dup_return(tracee);
	}
}


// ---- execve ----

static void handle_execve_return(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (g_atomic_int_get(tmps->share_count) > 1) {
		// Don't decrement before copy is made, otherwise other threads may
		// also decrement, share_count may reach zero and fd_table may be freed.
		GHashTable *fd_copy = fd_table_deep_copy(tmps->fd_table, tmps->fd_mutex);
		if (g_atomic_int_dec_and_test(tmps->share_count)) {
			// All other sharing partners unshared the table in the meantime.
			// Free old table, share count and fd_mutex can be reused.
			fd_table_free(tmps->fd_table);
		} else {
			// There are still other sharing partners left. Allocate new memory
			// for independent share_count and fd_mutex.
			tmps->share_count = malloc(sizeof(guint));
			*tmps->share_count = 0;
			tmps->fd_mutex = malloc(sizeof(GMutex));
			g_mutex_init(tmps->fd_mutex);
		}
		tmps->fd_table = fd_copy;
		g_atomic_int_inc(tmps->share_count);
	}
}

// TODO read args in call, not in return

// TODO handle CLOEXEC flag for fds

// TODO later:
// ---- eventfd2 ----
// ---- socket ----
// ---- socketpair ----


// ---- keep also track of time spent in syscalls  ----
// ---- that are not considered in file statistics ----

static void handle_unconsidered_call(pid_t tracee, int syscall) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->sc = syscall;
	if (clock_gettime(USED_CLOCK, &tmps->start_time)) {
		fprintf(stderr, "%s", "Error while reading start time of unconsidered "
		        "syscall");
		exit(1);
	}
}

static void handle_unconsidered_return(pid_t tracee) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "%s", "Error while reading end time of unconsidered "
		        "syscall");
		exit(1);
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	unsigned long long elapsed_ns = calc_elapsed_ns(&tmps->start_time, &current_time);
	syscall_stat_incr(tmps->sc, elapsed_ns);
}


void handle_syscall_call(pid_t tracee, int syscall) {
	// file statistics
	switch (syscall) {
		case SYS_open:
			handle_open_call(tracee);
			return;
		case SYS_openat:
			handle_openat_call(tracee);
			return;
		case SYS_close:
			handle_close_call(tracee);
			return;
		case SYS_read:
			handle_read_call(tracee);
			return;
		case SYS_write:
			handle_write_call(tracee);
			return;
		case SYS_pipe:
		case SYS_pipe2:
			handle_pipe_call(tracee);
			return;
	}

	// unconsidered syscalls
	switch (syscall) {
		case SYS_dup:
		case SYS_dup2:
		case SYS_dup3:
			handle_dup_call(tracee);
			break;
		case SYS_fcntl:
			handle_fcntl_call(tracee);
			break;
	}
	handle_unconsidered_call(tracee, syscall);
}

void handle_syscall_return(pid_t tracee, int syscall) {
	// file statistics
	switch (syscall) {
		case SYS_open:
			handle_open_return(tracee);
			return;
		case SYS_openat:
			handle_open_return(tracee);
			return;
		case SYS_close:
			handle_close_return(tracee);
			return;
		case SYS_read:
			handle_read_return(tracee);
			return;
		case SYS_write:
			handle_write_return(tracee);
			return;
		case SYS_pipe:
		case SYS_pipe2:
			handle_pipe_return(tracee);
			break;
	}

	// unconsidered syscalls
	handle_unconsidered_return(tracee);
	switch (syscall) {
		case SYS_dup:
		case SYS_dup2:
		case SYS_dup3:
			handle_dup_return(tracee);
			break;
		case SYS_fcntl:
			handle_fcntl_return(tracee);
			break;
		case SYS_execve:
			handle_execve_return(tracee);
			break;
	}
}

