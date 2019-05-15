#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <sys/socket.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
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

#define PIPE_R "pipe_r"
#define PIPE_W "pipe_w"
#define SOCKET "socket"
#define EVENTFD "eventfd"
#define EPOLL "epoll"
#define ACCEPT "accept"


static int read_string(pid_t tracee, char const *base, char *dest,
                       size_t const max_len) {
	size_t chars_per_word = sizeof(long) / sizeof(char);
	for (size_t i = 0; i * chars_per_word < max_len; i++) {
		errno = 0;
		long data = ptrace(PTRACE_PEEKDATA, tracee,
		                   base + (i * chars_per_word), NULL);
		if (data == -1 && errno != 0) {
			return -1;
		}
		memcpy(dest + (i * sizeof(long)), &data, sizeof(data));
		for (size_t j = 0; j < chars_per_word; j++) {
			if (dest[(i * chars_per_word) + j] == '\0') {
				return 0;
			}
		}
	}
	dest[max_len - 1] = 0;
	return 0;
}

static void save_current_time(struct timespec *start_time, int sc) {
	if (clock_gettime(USED_CLOCK, start_time)) {
		fprintf(stderr, "Error while reading start time of %s",
		        syscall_names[sc]);
		exit(1);
	}
}

static unsigned long long calc_elapsed_ns(pid_t tracee, thread_tmps **tmps, int sc) {
	struct timespec current_time;
	if (clock_gettime(USED_CLOCK, &current_time)) {
		fprintf(stderr, "Error while reading end time of %s", syscall_names[sc]);
		exit(1);
	}
	*tmps = thread_tmps_lookup(tracee);
	unsigned long long start_ns = (*tmps)->start_time.tv_sec * NANOS +
	                              (*tmps)->start_time.tv_nsec;
	unsigned long long current_ns = current_time.tv_sec * NANOS +
	                                current_time.tv_nsec;
	return current_ns - start_ns;
}


// ==== SYSCALL HANDLERS ====

// ---- open/openat ----

static void handle_open_call(pid_t tracee, int sc, bool openat) {
	int name_reg;
	int flags_reg;
	if (openat) {
		name_reg = RSI;
		flags_reg = RDX;
	} else {
		name_reg = RDI;
		flags_reg = RSI;
	}
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * flags_reg);
	char const *base = (char const *) ptrace(PTRACE_PEEKUSER, tracee,
	                                         sizeof(long) * name_reg);
	if (read_string(tracee, base, tmps->filename_buffer, FILENAME_BUFF_SIZE)) {
		fprintf(stderr, "Error while reading filename of %s", syscall_names[sc]);
		exit(1);
	}
	save_current_time(&tmps->start_time, sc);
}

static void handle_open_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd >= 0) {
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
		                tmps->filename_buffer, (tmps->int_a & O_CLOEXEC) != 0);
	}
	file_stat_incr_open(tmps->filename_buffer, elapsed_ns);
}


// ---- close ----

static void handle_close_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	save_current_time(&tmps->start_time, sc);
}

static void handle_close_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->int_a);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_close(filename, elapsed_ns);
	g_mutex_unlock(tmps->fd_mutex);
	fd_table_remove(tmps->fd_table, tmps->fd_mutex, tmps->int_a);
}


// ---- read ----

static void handle_read_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	save_current_time(&tmps->start_time, sc);
}

static void handle_read_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	ssize_t ret_bytes = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->int_a);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_read(filename, elapsed_ns, ret_bytes);
	g_mutex_unlock(tmps->fd_mutex);
}


// ---- write ----

static void handle_write_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	save_current_time(&tmps->start_time, sc);
}

static void handle_write_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	ssize_t ret_bytes = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	g_mutex_lock(tmps->fd_mutex);
	char const *filename = fd_table_lookup(tmps->fd_table, tmps->int_a);
	if (filename == NULL) {
		filename = "NULL";
	}
	file_stat_incr_write(filename, elapsed_ns, ret_bytes);
	g_mutex_unlock(tmps->fd_mutex);
}


// ---- pipe/pipe2 ----

static void handle_pipe_call(pid_t tracee, int sc, bool pipe2) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->ptr = (int *) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	if (pipe2) {
		tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	}
	save_current_time(&tmps->start_time, sc);
}

static void handle_pipe_return(pid_t tracee, int sc, bool pipe2) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	if (ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX) == 0) {
		int *pipefd = tmps->ptr;
		int pipefd_read = ptrace(PTRACE_PEEKDATA, tracee, pipefd, NULL);
		int pipefd_write = ptrace(PTRACE_PEEKDATA, tracee, pipefd + 1, NULL);
		if (pipe2) {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_read,
			                PIPE_R, (tmps->int_a & O_CLOEXEC) != 0);
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_write,
			                PIPE_W, (tmps->int_a & O_CLOEXEC) != 0);
		} else {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_read,
			                PIPE_R, false);
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, pipefd_write,
			                PIPE_W, false);
		}
		file_stat_incr_open(PIPE_R, elapsed_ns/2);
		file_stat_incr_open(PIPE_W, elapsed_ns/2);
	}
}


// ---- socket ----

static void handle_socket_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	save_current_time(&tmps->start_time, sc);
}

static void handle_socket_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd >= 0) {
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
		                SOCKET, (tmps->int_a & SOCK_CLOEXEC) != 0);
	}
	file_stat_incr_open(SOCKET, elapsed_ns);
}


// ---- socketpair ----

static void handle_socketpair_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->ptr = (int *) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * R10);
	save_current_time(&tmps->start_time, sc);
}

static void handle_socketpair_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	if (ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX) == 0) {
		int *socket_pair = tmps->ptr;
		int socket_a = ptrace(PTRACE_PEEKDATA, tracee, socket_pair, NULL);
		int socket_b = ptrace(PTRACE_PEEKDATA, tracee, socket_pair + 1, NULL);
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, socket_a, SOCKET, false);
		fd_table_insert(tmps->fd_table, tmps->fd_mutex, socket_b, SOCKET, false);
		file_stat_incr_open(SOCKET, elapsed_ns/2);
		file_stat_incr_open(SOCKET, elapsed_ns/2);
	}
}


// ---- eventfd/eventfd2 ----

static void handle_eventfd_call(pid_t tracee, int sc, bool eventfd2) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (eventfd2) {
		tmps->int_a = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	}
	save_current_time(&tmps->start_time, sc);
}

static void handle_eventfd_return(pid_t tracee, int sc, bool eventfd2) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd >= 0) {
		if (eventfd2) {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
			                EVENTFD, (tmps->int_a & EFD_CLOEXEC) != 0);
		} else {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd, EVENTFD, false);
		}
	}
	file_stat_incr_open(EVENTFD, elapsed_ns);
}


// ---- epoll_create/epoll_create1 ----

static void handle_epollcreate_call(pid_t tracee, int sc, bool epoll_create1) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (epoll_create1) {
		tmps->int_a = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	}
	save_current_time(&tmps->start_time, sc);
}

static void handle_epollcreate_return(pid_t tracee, int sc, bool epoll_create1) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd >= 0) {
		if (epoll_create1) {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
			                EPOLL, (tmps->int_a & EPOLL_CLOEXEC) != 0);
		} else {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd, EPOLL, false);
		}
	}
	file_stat_incr_open(EPOLL, elapsed_ns);
}


// ---- accept/accept4 ----

static void handle_accept_call(pid_t tracee, int sc, bool accept4) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	if (accept4) {
		tmps->int_a = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * R10);
	}
	save_current_time(&tmps->start_time, sc);
}

static void handle_accept_return(pid_t tracee, int sc, bool accept4) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	long ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd >= 0) {
		if (accept4) {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd,
			                ACCEPT, (tmps->int_a & SOCK_CLOEXEC) != 0);
		} else {
			fd_table_insert(tmps->fd_table, tmps->fd_mutex, ret_fd, ACCEPT, false);
		}
	}
	file_stat_incr_open(ACCEPT, elapsed_ns);
}


// ==== UNCONSIDERED SYSCALLS ====

// ---- dup/dup2/dup3 ----

static void handle_dup_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
}

static void handle_dup_return(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	int ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (tmps->int_a >= 0 && tmps->int_a != ret_fd) {
		fd_table_insert_dup(tmps->fd_table, tmps->fd_mutex, tmps->int_a,
		                    ret_fd, false, false);
	}
}


// ---- fcntl ----

static void handle_fcntl_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	tmps->int_b = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
	tmps->int_c = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDX);
}

static void handle_fcntl_return(pid_t tracee) {
	int ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (ret_fd != -1) {
		thread_tmps *tmps = thread_tmps_lookup(tracee);
		if (tmps->int_b == F_DUPFD) {
			fd_table_insert_dup(tmps->fd_table, tmps->fd_mutex, tmps->int_a,
			                    ret_fd, false, false);
		} else if (tmps->int_b == F_DUPFD_CLOEXEC) {
			fd_table_insert_dup(tmps->fd_table, tmps->fd_mutex, tmps->int_a,
			                    ret_fd, true, false);
		} else if (tmps->int_b == F_SETFD) {
			fd_table_set_cloexec(tmps->fd_table, tmps->fd_mutex, tmps->int_a,
			                     (tmps->int_c & FD_CLOEXEC) != 0);
		}
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
	fd_table_remove_cloexec(tmps->fd_table, tmps->fd_mutex);
}


// ---- ioctl ----

static void handle_ioctl_call(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
    tmps->int_a = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
	tmps->int_b = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RSI);
}

static void handle_ioctl_return(pid_t tracee) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	int ret_fd = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	if (tmps->int_b & TIOCGPTPEER) {
		fd_table_insert_dup(tmps->fd_table, tmps->fd_mutex, tmps->int_a,
		                    ret_fd, false, true);
	}
}


// ---- keep also track of time spent in syscalls  ----
// ---- that are not considered in file statistics ----

static void handle_unconsidered_call(pid_t tracee, int sc) {
	thread_tmps *tmps = thread_tmps_lookup(tracee);
	save_current_time(&tmps->start_time, sc);
}

static void handle_unconsidered_return(pid_t tracee, int sc) {
	thread_tmps *tmps;
	unsigned long long elapsed_ns = calc_elapsed_ns(tracee, &tmps, sc);
	syscall_stat_incr(sc, elapsed_ns);
}


void handle_syscall_call(pid_t tracee, int sc) {
	// file statistics
	switch (sc) {
		case SYS_open:
			handle_open_call(tracee, sc, false);
			return;
		case SYS_openat:
			handle_open_call(tracee, sc, true);
			return;
		case SYS_close:
			handle_close_call(tracee, sc);
			return;
		case SYS_read:
			handle_read_call(tracee, sc);
			return;
		case SYS_write:
			handle_write_call(tracee, sc);
			return;
		case SYS_pipe:
			handle_pipe_call(tracee, sc, false);
			return;
		case SYS_pipe2:
			handle_pipe_call(tracee, sc, true);
			return;
		case SYS_socket:
			handle_socket_call(tracee, sc);
			return;
		case SYS_socketpair:
			handle_socketpair_call(tracee, sc);
			return;
		case SYS_eventfd:
			handle_eventfd_call(tracee, sc, false);
			return;
		case SYS_eventfd2:
			handle_eventfd_call(tracee, sc, true);
			return;
		case SYS_epoll_create:
			handle_epollcreate_call(tracee, sc, false);
			return;
		case SYS_epoll_create1:
			handle_epollcreate_call(tracee, sc, true);
			return;
		case SYS_accept:
			handle_accept_call(tracee, sc, false);
			return;
		case SYS_accept4:
			handle_accept_call(tracee, sc, true);
			return;
	}

	// unconsidered syscalls
	switch (sc) {
		case SYS_dup:
		case SYS_dup2:
		case SYS_dup3:
			handle_dup_call(tracee);
			break;
		case SYS_fcntl:
			handle_fcntl_call(tracee);
			break;
		case SYS_ioctl:
			handle_ioctl_call(tracee);
			break;
	}
	handle_unconsidered_call(tracee, sc);
}

void handle_syscall_return(pid_t tracee, int sc) {
	// file statistics
	switch (sc) {
		case SYS_open:
			handle_open_return(tracee, sc);
			return;
		case SYS_openat:
			handle_open_return(tracee, sc);
			return;
		case SYS_close:
			handle_close_return(tracee, sc);
			return;
		case SYS_read:
			handle_read_return(tracee, sc);
			return;
		case SYS_write:
			handle_write_return(tracee, sc);
			return;
		case SYS_pipe:
			handle_pipe_return(tracee, sc, false);
			return;
		case SYS_pipe2:
			handle_pipe_return(tracee, sc, true);
			return;
		case SYS_socket:
			handle_socket_return(tracee, sc);
			return;
		case SYS_socketpair:
			handle_socketpair_return(tracee, sc);
			return;
		case SYS_eventfd:
			handle_eventfd_return(tracee, sc, false);
			return;
		case SYS_eventfd2:
			handle_eventfd_return(tracee, sc, true);
			return;
		case SYS_epoll_create:
			handle_epollcreate_return(tracee, sc, false);
			return;
		case SYS_epoll_create1:
			handle_epollcreate_return(tracee, sc, true);
			return;
		case SYS_accept:
			handle_accept_return(tracee, sc, false);
			return;
		case SYS_accept4:
			handle_accept_return(tracee, sc, true);
			return;
	}

	// unconsidered syscalls
	handle_unconsidered_return(tracee, sc);
	switch (sc) {
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
		case SYS_ioctl:
			handle_ioctl_return(tracee);
			break;
	}
}

