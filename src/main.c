#define _GNU_SOURCE

#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <glib.h>
#include <time.h>
#include <sys/syscall.h>
#include <sched.h>

#include "syscall_handler.h"
#include "file_stat.h"
#include "fd_table.h"
#include "json_printer.h"
#include "unconsidered_syscall_stat.h"
#include "thread_temporaries.h"


static bool verbose = false;
static GOptionEntry entries[] = {
	{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
	{ NULL }
};

static volatile guint active_threads = 0;
static GMutex threads_mutex;
static GList *threads = NULL;

static volatile int err = 0;

static int start_tracee(int argc, char **argv) {
	char *args [argc+1];
	memcpy(args, argv, argc * sizeof(char*));
	args[argc] = NULL;

	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP); // notify parent that tracing can start
	return execvp(args[0], args);
}

static void start_tracer(pid_t tracee);

typedef struct {
	pid_t tracee;
	GHashTable *fd_table;
	guint *share_count;
	GMutex *fd_mutex;
} thread_start_data;

static gpointer thread_func(gpointer data_ptr) {
	thread_start_data *data = data_ptr;
	ptrace(PTRACE_ATTACH, data->tracee);
	thread_tmps_insert(data->tracee, data->fd_table, data->share_count,
	                   data->fd_mutex);
	start_tracer(data->tracee);

	g_atomic_int_dec_and_test(&active_threads);
	free(data);
	return NULL;
}

static void threads_add(pid_t tracee, pid_t parents_tracee, int clone_flags) {
	g_atomic_int_inc(&active_threads);

	thread_start_data *data = malloc(sizeof(thread_start_data));
	data->tracee = tracee;
	thread_tmps *tmps = thread_tmps_lookup(parents_tracee);
	if (clone_flags & CLONE_FILES) {
		// shared file descriptor table
		data->fd_table = tmps->fd_table;
		data->share_count = tmps->share_count;
		data->fd_mutex = tmps->fd_mutex;
	} else {
		// unshared file descriptor table
		data->fd_table = fd_table_deep_copy(tmps->fd_table, tmps->fd_mutex);
		data->share_count = NULL;
		data->fd_mutex = NULL;
	}

	GThread *thread = g_thread_new(NULL, thread_func, data);
	g_mutex_lock(&threads_mutex);
	threads = g_list_prepend(threads, thread);
	g_mutex_unlock(&threads_mutex);
}

static int wait_for_syscall(pid_t tracee) {
	int status;
	ptrace(PTRACE_SYSCALL, tracee, 0, 0);
	while (1) {
		waitpid(tracee, &status, __WALL);
		if (status >> 8 == (SIGTRAP | (PTRACE_EVENT_CLONE << 8)) ||
		    status >> 8 == (SIGTRAP | (PTRACE_EVENT_FORK << 8)) ||
		    status >> 8 == (SIGTRAP | (PTRACE_EVENT_VFORK << 8))) {
			int clone_flags = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RDI);
			pid_t new_child;
			ptrace(PTRACE_GETEVENTMSG, tracee, NULL, (long) &new_child);
			if (new_child >= 0) {
				waitpid(new_child, &status, __WALL);
				ptrace(PTRACE_DETACH, new_child, NULL, SIGSTOP);
				threads_add(new_child, tracee, clone_flags);
			}
		}

		if (WIFSTOPPED(status) && WSTOPSIG(status) == (SIGTRAP | 0x80)) {
			// stopped by a syscall
			return 0;
		}

		if (WIFEXITED(status)) {
			// error or tracee exited
			return 1;
		}

		if (WIFSTOPPED(status)) {
			// signal delivery stop
			ptrace(PTRACE_SYSCALL, tracee, 0, WSTOPSIG(status));
		} else {
			ptrace(PTRACE_SYSCALL, tracee, 0, 0);
		}
	}
}

static void start_tracer(pid_t tracee) {
	int status;
	waitpid(tracee, &status, 0);
	ptrace(PTRACE_SETOPTIONS, tracee, NULL,
	       PTRACE_O_TRACESYSGOOD |  // get syscall info
	       PTRACE_O_TRACECLONE |    // trace cloned processes
	       PTRACE_O_TRACEFORK |     // trace forked processes
	       PTRACE_O_TRACEVFORK |    // trace vforked processes
	       PTRACE_O_TRACEEXEC |     // disable legacy sigtrap on execve
	       PTRACE_O_EXITKILL);      // send SIGKILL to tracee if tracer exits

	int syscall;
	while (1) {
		// syscall call
		if (wait_for_syscall(tracee)) {
			break;
		}
		syscall = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * ORIG_RAX);
		handle_syscall_call(tracee, syscall);

		// syscall return
		if (wait_for_syscall(tracee)) {
			break;
		}
		handle_syscall_return(tracee, syscall);
	}

	thread_tmps_remove(tracee);
}

static void get_mpi_rank(char **rank) {
	if ((*rank = getenv("PMI_RANK"))) {
		return;
	} else if ((*rank = getenv("PMIX_RANK"))) {
		return;
	} else if ((*rank = getenv("OMPI_COMM_WORLD_RANK"))) {
		return;
	} else {
		*rank = getenv("ALPS_APP_PE");
	}
}

static int main_tracer(pid_t tracee, char const *trace_id) {
	thread_tmps_init();
	file_stat_init();
	syscall_stat_init();

	GHashTable *main_fd_table = fd_table_create();
	thread_tmps_insert(tracee, main_fd_table, NULL, NULL);
	start_tracer(tracee);

	// wait for remaining threads
	const struct timespec sleep_interval = {0, 200000000L};
	while (1) {
		if (g_atomic_int_get(&active_threads) == 0) {
			break;
		}
		nanosleep(&sleep_interval, NULL);
	}

	// join threads
	g_mutex_lock(&threads_mutex);
	if (threads != NULL) {
		GList *list = threads;
		do {
			g_thread_join((GThread *) list->data);
		} while ((list = (GList *) g_list_next(list)) != NULL);
	}
	g_mutex_unlock(&threads_mutex);

	// print stats
	printf("\n");
	if (verbose) {
		file_stat_print_all();
		syscall_stat_print_all();
	}

	// print json
	char hostname[256];
	gethostname(hostname, 256);
	char *mpi_rank;
	get_mpi_rank(&mpi_rank);
	char filename[strlen(trace_id) + strlen(hostname) + 12];
	sprintf(filename, "%s_%s_%s.json", trace_id, hostname,
	        mpi_rank ? mpi_rank : "NULL");
	if (print_stats_as_json(filename, hostname, mpi_rank)) {
		printf("File statistics were written to '%s'\n", filename);
		printf("Run 'iotrace --help' to get information about the JSON output "
		       "format\n");
	} else {
		fprintf(stderr, "\nError, JSON file could not be created\n");
		err = 1;
	}

	g_list_free(threads);
	thread_tmps_free();
	file_stat_free();
	syscall_stat_free();
	return err;
}

int main(int argc, char **argv) {
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new("TRACE_ID PROG [PROG_ARGS\u2026]");
	g_option_context_set_summary(context, "Analyzes the I/O behavior of an "
			"executable and prints the results to JSON files.\n"
			"TRACE_ID: The identifier/name of the trace, which is part of the "
			"output files' names.\n"
			"PROG: The executable that should be analyzed.\n"
			"PROG_ARGS: The program's arguments.\n"
			"\n"
			"The JSON output files are formatted as follows:\n"
			"open : [ 'count', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"close : [ 'count', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"read : [ 'total bytes', 'total nanosecs', 'min byte/sec', "
			"'max byte/sec' ]\n"
			"read-blocks : [ [ 'number of bytes', 'count' ], ... ]\n"
			"write : [ 'total bytes', 'total nanosecs', 'min byte/sec', "
			"'max byte/sec' ]\n"
			"write-blocks : [ [ 'number of bytes', 'count' ], ... ]");
	g_option_context_set_strict_posix(context, true);
	g_option_context_add_main_entries(context, entries, NULL);
	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		fprintf(stderr, "Option parsing failed: %s\n", error->message);
		exit(1);
	} else if (argc < 3) {
		printf("%s", g_option_context_get_help(context, FALSE, NULL));
		exit(1);
	}
	g_option_context_free(context);

	pid_t pid = fork();
	if (pid == 0) {
		return start_tracee(argc - 2, argv + 2);
	} else {
		return main_tracer(pid, argv[1]);
	}
}

