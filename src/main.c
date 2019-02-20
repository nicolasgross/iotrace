#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>

#include "syscall_handler.h"
#include "file_statistics.h"
#include "fd_table.h"


static int start_tracee(int argc, char **argv) {
	char *args [argc+1];
	memcpy(args, argv, argc * sizeof(char*));
	args[argc] = NULL;

	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP); // notify parent that tracing can start
	return execvp(args[0], args);
}

static int wait_for_syscall(pid_t tracee) {
	int status;
	while (1) {
		ptrace(PTRACE_SYSCALL, tracee, 0, 0); // run until syscall
		waitpid(tracee, &status, __WALL);
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) {
			// stopped by a syscall
			return 0;
		}
		// TODO handle PID of forked/vforked/cloned process and create new
		// thread that runs 'start_tracer'
		if (WIFEXITED(status)) {
			// tracee exited
			return 1;
		}
    }
}

static int start_tracer(pid_t tracee) {
	fd_table table = fd_table_create();
	int status;
	int syscall;
	waitpid(tracee, &status, 0); // wait for notification
	ptrace(PTRACE_SETOPTIONS, tracee, 0,
	       PTRACE_O_TRACECLONE | // trace cloned processes
	       PTRACE_O_TRACEFORK | // trace forked processes
	       PTRACE_O_TRACEVFORK | // trace vforked processes
	       PTRACE_O_TRACESYSGOOD | // get syscall info
	       PTRACE_O_EXITKILL); // send SIGKILL to tracee if tracer exits
	while(1) {
		if (wait_for_syscall(tracee) != 0) {
			break;
		}
		// syscall call
		syscall = (int) ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * ORIG_RAX);
		handle_syscall_call(tracee, syscall);

		if (wait_for_syscall(tracee) != 0) {
			break;
		}
		// syscall return
		handle_syscall_return(tracee, table, syscall);
	}
	fd_table_free(table);
	return 0;
}

static int main_tracer(int pid) {
	file_stat_init();
	int err = start_tracer(pid);
	// TODO Wait for remaining threads
	file_stat_print_all();
	// TODO print statistics as json
	file_stat_free();
	return err;
}

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s prog args\n", argv[0]);
		exit(1);
	}

	pid_t pid = fork();
	if (pid == 0) {
		return start_tracee(argc-1, argv+1);
	} else {
		return main_tracer(pid);
	}
}

