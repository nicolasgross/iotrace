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

#include "syscall_handler.h"
#include "file_stat.h"
#include "fd_table.h"
#include "json_printer.h"
#include "unconsidered_syscall_stat.h"


static bool verbose = false;
static GOptionEntry entries[] = {
	{ "verbose", 'v', 0, G_OPTION_ARG_NONE, &verbose, "Be verbose", NULL },
	{ NULL }
};


static int start_tracee(int argc, char **argv) {
	char *args [argc+1];
	memcpy(args, argv, argc * sizeof(char*));
	args[argc] = NULL;

	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP); // notify parent that tracing can start
	return execvp(args[0], args);
}

static pid_t wait_for_syscall(pid_t last_stopped) {
	int status;
	pid_t new_stopped;
	ptrace(PTRACE_SYSCALL, last_stopped, NULL, NULL);
	while (1) {
		new_stopped = waitpid(-1, &status, __WALL);
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) {
			// stopped by a syscall
			return new_stopped;
		}
		// TODO handle PID of forked/vforked/cloned process and create new
		// thread that runs 'start_tracer'
		if (new_stopped == -1 || WIFEXITED(status)) {
			// TODO check whether last tracer, only exit if true
			// error or tracee exited
			return -1;
		}
		ptrace(PTRACE_SYSCALL, new_stopped, NULL, NULL);
	}
}

static int start_tracer(pid_t tracee) {
	// TODO one fd_table per child
	fd_table table = fd_table_create();
	int status;
	int syscall;
	pid_t stopped_child = tracee;
	waitpid(tracee, &status, 0); // wait for notification
	ptrace(PTRACE_SETOPTIONS, tracee, NULL,
	       PTRACE_O_TRACESYSGOOD | // get syscall info
	       PTRACE_O_TRACECLONE | // trace cloned processes
	       PTRACE_O_TRACEFORK | // trace forked processes
	       PTRACE_O_TRACEVFORK | // trace vforked processes
	       PTRACE_O_EXITKILL); // send SIGKILL to tracee if tracer exits
	while (1) {
		// syscall call
		if ((stopped_child = wait_for_syscall(stopped_child)) == -1) {
			break;
		}
		syscall = (int) ptrace(PTRACE_PEEKUSER, stopped_child,
		                       sizeof(long) * ORIG_RAX);
		handle_syscall_call(stopped_child, syscall);

		// syscall return
		if ((stopped_child = wait_for_syscall(stopped_child)) == -1) {
			break;
		}
		handle_syscall_return(stopped_child, table, syscall);
	}
	fd_table_free(table);
	return 0;
}

static int main_tracer(int pid, char const *json_filename) {
	file_stat_init();
	syscall_stat_init();
	int err = start_tracer(pid);
	// TODO Wait for remaining threads
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

