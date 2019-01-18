#include <stdio.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/syscall.h>

#include "syscall_handler.h"


static const char *const syscall_to_string(int syscall) {
	// TODO
	return "openat";
}

// ---- openat ----

static void handle_openat_call(pid_t tracee) {
	printf("%s", syscall_to_string(SYS_openat));
	// TODO print arguments '(...) ='
	printf("%s", "(args) = ");
}

static void handle_openat_return(pid_t tracee) {
	int retval = ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
	printf("%d\n", retval);
}


void handle_syscall_call(pid_t tracee, int syscall) {
	switch(syscall) {
		case SYS_openat:
			handle_openat_call(tracee);
			break;
	}
}

void handle_syscall_return(pid_t tracee, int syscall) {
	switch(syscall) {
		case SYS_openat:
			handle_openat_return(tracee);
			break;
	}
}

