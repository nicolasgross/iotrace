#define _POSIX_SOURCE
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>


int do_child(int argc, char **argv);
int do_tracer(pid_t child);
int wait_for_syscall(pid_t child);

int main(int argc, char **argv) {
	if (argc < 2) {
		fprintf(stderr, "Usage: %s prog args\n", argv[0]);
		exit(1);
	}

	pid_t pid = fork();
	if (pid == 0) {
		// child
		return do_child(argc-1, argv+1);
	} else {
		// parent
		return do_tracer(pid);
	}
}

int do_child(int argc, char **argv) {
	char *args [argc+1];
	memcpy(args, argv, argc * sizeof(char*));
	args[argc] = NULL;

	ptrace(PTRACE_TRACEME);
	kill(getpid(), SIGSTOP); // notify parent that tracing can start
	return execvp(args[0], args);
}

int do_tracer(pid_t tracee) {
	int status;
	int syscall;
	int retval;
	waitpid(tracee, &status, 0); // wait for notification
	ptrace(PTRACE_SETOPTIONS, tracee, 0,
	       PTRACE_O_TRACECLONE | // trace cloned processes
	       PTRACE_O_TRACEFORK | // trace forked processes
	       PTRACE_O_TRACEVFORK | // trace vforked processes
	       PTRACE_O_TRACESYSGOOD | // get syscall info
	       PTRACE_O_EXITKILL); // send SIGKILL to tracee if tracer exits
	while(1) {
		if (wait_for_syscall(tracee) != 0) break;

		// syscall call
		syscall = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * ORIG_RAX);
		fprintf(stderr, "syscall(%d) = ", syscall);

		if (wait_for_syscall(tracee) != 0) break;

		// syscall return value
		retval = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
		fprintf(stderr, "%d\n", retval);
	}
	return 0;
}

int wait_for_syscall(pid_t tracee) {
	int status;
	while (1) {
		ptrace(PTRACE_SYSCALL, tracee, 0, 0); // run until syscall
		waitpid(tracee, &status, __WALL);
		if (WIFSTOPPED(status) && WSTOPSIG(status) & 0x80) {
			// stopped by a syscall
			return 0;
		}
		// TODO handle PID of forked/vforked/cloned process
		if (WIFEXITED(status)) {
			// tracee exited
			return 1;
		}
    }
}
