#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/syscall.h>
#include <sys/user.h>
#include <linux/fcntl.h>

#include "syscall_handler.h"
#include "syscall_names.h"
#include "fd_table.h"


static struct user_regs_struct regs;

// TODO store open information in struct


static const char *const syscall_to_string(int syscall) {
	return syscall_names[syscall];
}

static void read_regs(pid_t tracee) {
	if (ptrace(PTRACE_GETREGS, tracee, NULL, &regs)) {
		fprintf(stderr, "%s", "Error while reading register values");
		exit(1);
	}
}

static void read_string(pid_t tracee, unsigned long base, char *dest,
                        const size_t len) {
	for (size_t i = 0; i * 8 < len; i++) {
		long data = ptrace(PTRACE_PEEKDATA, tracee,
		                   base + (i * sizeof(long)), NULL);
		if (data == -1) {
			fprintf(stderr, "%s", "Error while reading a string");
			exit(1);
		}
		memcpy(dest + (i * sizeof(long)), &data, sizeof(data));
		if (dest[i] == 0 || dest[i + 1] == 0 || dest[i + 2] == 0 ||
		    dest[i + 3] == 0 || dest[i + 4] == 0 || dest[i + 5] == 0 ||
		    dest[i + 6] == 0 || dest[i + 7] == 0) {
			return;
		}
	}
	dest[len - 1] = 0;
}

static long get_retval(pid_t tracee) {
	return ptrace(PTRACE_PEEKUSER, tracee, sizeof(long) * RAX);
}


// ---- openat ----

static void handle_openat_call(pid_t tracee, int syscall) {
	// TODO store filename, time
}

static void handle_openat_return(pid_t tracee) {
	// TODO store fd, time
	// TODO add fd/filename to mapping table
	// TODO update file stat open count
}

// TODO now:
// ---- open ----
// ---- open_from ----
// ---- close ----
// ---- read ----
// ---- write ----

// TODO later:
// ---- execve ----
// ---- dup2 ----
// ---- eventfd2 ----
// ---- socket ----
// ---- socketpair ----
// ---- pipe ----


// ---- unmatched ----

static void handle_unmatched_call(pid_t tracee, int syscall) {
	printf("%s(...) = ", syscall_to_string(syscall));
}

static void handle_unmatched_return(pid_t tracee) {
	printf("%d\n", (int) get_retval(tracee));
}


void handle_syscall_call(pid_t tracee, fd_table table, int syscall) {
	switch(syscall) {
		case SYS_openat:
			handle_openat_call(tracee, syscall);
			break;
		default:
			handle_unmatched_call(tracee, syscall);
			break;
	}
}

void handle_syscall_return(pid_t tracee, fd_table table, int syscall) {
	switch(syscall) {
		case SYS_openat:
			handle_openat_return(tracee);
			break;
		default:
			handle_unmatched_return(tracee);
			break;
	}
}

