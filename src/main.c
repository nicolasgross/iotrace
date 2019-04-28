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

static pid_t main_pid;
static volatile gint active_threads = 0;
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

static gpointer thread_func(gpointer tracee) {
	thread_tmps_insert(getpid());
	pid_t tracee_pid = *(pid_t *) tracee;
	ptrace(PTRACE_ATTACH, tracee_pid);
	int status;
	waitpid(tracee_pid, &status, 0);
	ptrace(PTRACE_SETOPTIONS, tracee_pid, NULL,
	       PTRACE_O_TRACESYSGOOD |  // get syscall info
	       PTRACE_O_EXITKILL);      // send SIGKILL to tracee if tracer exits
	start_tracer(tracee_pid);
	g_atomic_int_dec_and_test(&active_threads);
	free(tracee);
	return NULL;
}

static void threads_add(pid_t tracee) {
	g_atomic_int_inc(&active_threads);
	g_mutex_lock(&threads_mutex);
	pid_t *tracee_mem = malloc(sizeof(pid_t));
	*tracee_mem = tracee;
	GThread *thread = g_thread_new(NULL, thread_func, tracee_mem);
	threads = g_list_prepend(threads, thread);
	g_mutex_unlock(&threads_mutex);
}

static int wait_for_syscall(pid_t tracee, int syscall) {
	int status;
	ptrace(PTRACE_SYSCALL, tracee, NULL, NULL);
	while (1) {
		waitpid(tracee, &status, __WALL);
		if (syscall == SYS_clone) {
			// syscall return from clone
			pid_t new_child = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
			if (new_child != -1) {
				threads_add(new_child);
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

		ptrace(PTRACE_SYSCALL, tracee, NULL, NULL);
	}
}

static void start_tracer(pid_t tracee) {
	int syscall;
	while (1) {
		// syscall call
		if (wait_for_syscall(tracee, -1)) {
			break;
		}
		syscall = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * ORIG_RAX);
		handle_syscall_call(tracee, syscall);

		// syscall return
		if (wait_for_syscall(tracee, syscall)) {
			break;
		}
		handle_syscall_return(tracee, syscall);
	}
}

static int main_tracer(pid_t tracee, char const *json_filename) {
	main_pid = getpid();
	fd_table_init();
	thread_tmps_init();
	thread_tmps_insert(getpid());
	file_stat_init();
	syscall_stat_init();
	int status;
	waitpid(tracee, &status, 0);    // wait for notification
	ptrace(PTRACE_SETOPTIONS, tracee, NULL,
	       PTRACE_O_TRACESYSGOOD |  // get syscall info
	       PTRACE_O_EXITKILL);      // send SIGKILL to tracee if tracer exits
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
	g_list_foreach(threads, (GFunc) g_thread_join, NULL);
	g_mutex_unlock(&threads_mutex);

	printf("\n");
	if (verbose) {
		file_stat_print_all();
		syscall_stat_print_all();
	}

	if (print_stats_as_json(json_filename)) {
		printf("File statistics were written to '%s'\n", json_filename);
		printf("Run 'iotrace --help' to get information about the JSON output "
		       "format\n");
	} else {
		fprintf(stderr, "\nError, JSON file could not be created\n");
		err = 1;
	}

	g_list_free(threads);
	fd_table_free();
	thread_tmps_free();
	file_stat_free();
	syscall_stat_free();
	return err;
}

int main(int argc, char **argv) {
	GError *error = NULL;
	GOptionContext *context;

	context = g_option_context_new("OUTPUT_FILE PROG [PROG_ARG\u2026]");
	g_option_context_set_summary(context, "Analyzes the I/O behavior of a "
			"program and prints the results to a JSON file. The analysis "
			"comprises ... TBD ...\n" // TODO explain full functionality
			"The argument OUTPUT_FILE is the location of the JSON output "
			"file, PROG is the program that should be analyzed and "
			"[PROG_ARG\u2026] are its arguments.\n"
			"\n"
			"The JSON output is formatted as follows:\n"
			"open : [ 'count', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"close : [ 'count', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"read : [ 'total bytes', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"read-blocks : [ [ 'number of bytes', 'count' ], ... ]\n"
			"write : [ 'total bytes', 'total nanosecs', 'min nanosecs', "
			"'max nanosecs' ]\n"
			"write-blocks : [ [ 'number of bytes', 'count' ], ... ]\n");
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

